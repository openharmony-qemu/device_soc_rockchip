/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * net_dwmac.c - real-time NIC data plane for the AMP slave core on the
 * RK3568 Synopsys dwmac-4.20a GMAC1 (fe010000, RGMII, YT8531S PHY).
 * Protocol-agnostic: EtherCAT, proprietary or raw-Ethernet users all ride
 * the same frame-level contract (net.h, v3 Net* surface).
 *
 * Division of labor: the Linux master (mcs_net.ko) owns clocks, reset,
 * pinmux/GRF, PHY config/MDIO and link polling; this file owns the RX/TX
 * data path and its interrupt (LOS ArchHwiCreate configures the GICD for
 * the owned SPI - group/route/trigger/priority, per-core ownership):
 *
 *   NetInit   - validate config (port/mode/link policy), optionally
 *                 force the PHY via MDIO, reset the DMA, program MTL/DMA
 *                 chan 0, set the rx filter, build the descriptor rings
 *                 (static, zero dynamic allocation). Legal in READY =
 *                 reconfigure; RUNNING must Stop first.
 *   NetStart  - mode-aware bring-up: IRQ/HYBRID register INTID 64 via
 *                 ArchHwiCreate (level-triggered; the gic-v3 port routes
 *                 the SPI to this core) and arm DMA interrupts; POLL
 *                 skips both (MAC silent on the wire, Recv() drives)
 *   NetSend   - copy in, own descriptor, ring doorbell (tail ptr);
 *                 TX completion is reaped lazily (no TIE)
 *   NetRecv   - dequeue from the ready queue, copy out, re-arm the
 *                 descriptor; in POLL/HYBRID it reaps the RX ring first
 *   NetStop   - quiesce within a bounded timeout (DMA_DEBUG_STATUS),
 *                 deregister the IRQ (interrupt modes), reset the rings
 *
 * Memory model: the DMA pools live in the dedicated Normal Non-Cacheable
 * window at NET_DMA_BASE (mapped by los_mmu.c), so PA==VA holds and
 * the descriptors/buffers are DMA-coherent without cache maintenance
 * even with the slave D-cache enabled. NetInit explicitly (re)writes
 * every descriptor, so the pools do not rely on .bss zeroing. All ring
 * state is static BSS.
 */

#include <stdint.h>
#include <string.h>
#include "los_compiler.h"
#include "los_debug.h"
#include "los_interrupt.h"
#include "los_arch_interrupt.h"
#include "net.h"
#include "net_ctrl.h"
#include "gmac1_profile.h"

/* ------------------------------------------------------------------ */
/* Register map (dwmac-4.20a, all offsets relative to GMAC1_BASE)      */
/* ------------------------------------------------------------------ */

/* MAC */
#define GMAC_CONFIG             0x0000u
#define GMAC_PACKET_FILTER      0x0008u
#define GMAC_ADDR0_HIGH         0x0300u
#define GMAC_ADDR0_LOW          0x0304u

#define GMAC_CONFIG_IPC         (1u << 27)
#define GMAC_CONFIG_BE          (1u << 18)
#define GMAC_CONFIG_JD          (1u << 17)
#define GMAC_CONFIG_PS          (1u << 15)
#define GMAC_CONFIG_FES         (1u << 14)
#define GMAC_CONFIG_DM          (1u << 13)
#define GMAC_CONFIG_DCRS        (1u << 9)
#define GMAC_CONFIG_TE          (1u << 1)
#define GMAC_CONFIG_RE          (1u << 0)

#define GMAC_PACKET_FILTER_PR   (1u << 0)
#define GMAC_PACKET_FILTER_HMC  (1u << 2)
#define GMAC_PACKET_FILTER_PM   (1u << 4)
#define GMAC_PACKET_FILTER_PCF  (1u << 7)

/* MTL */
#define MTL_OPERATION_MODE      0x0c00u
#define MTL_RXQ_DMA_MAP0        0x0c30u
#define MTL_TXQ0_OP_MODE        0x0d00u
#define MTL_RXQ0_OP_MODE        0x0d30u

#define MTL_OP_SCHALG_SP        (0x3u << 5)
#define MTL_OP_RAA_WSP          (1u << 2)
#define MTL_OP_MODE_RSF         (1u << 5)
#define MTL_OP_MODE_TXQEN       (1u << 3)
#define MTL_OP_MODE_TSF         (1u << 1)
#define MTL_OP_MODE_TTC_32      0u
#define MTL_OP_MODE_RTC_64      0u

/* DMA */
#define DMA_BUS_MODE            0x1000u
#define DMA_SYS_BUS_MODE        0x1004u
#define DMA_DEBUG_STATUS_0      0x100cu

#define DMA_BUS_MODE_SFT_RESET  (1u << 0)

#define DMA_SYS_BUS_MODE_EAME   (1u << 11)
#define DMA_SYS_BUS_MODE_FB     (1u << 0)
#define DMA_SYS_BUS_BLEN4       (1u << 1)
#define DMA_SYS_BUS_BLEN8       (1u << 2)
#define DMA_SYS_BUS_BLEN16      (1u << 3)
#define DMA_SYS_BUS_BLEN32      (1u << 4)

/* DMA channel 0 register block (0x1100 + ch * 0x80, ch = 0 here) */
#define DMA_CHAN_TX_CONTROL     0x1104u
#define DMA_CHAN_RX_CONTROL     0x1108u
#define DMA_CHAN_TX_BASE        0x1114u
#define DMA_CHAN_RX_BASE        0x111cu
#define DMA_CHAN_TX_END_ADDR    0x1120u   /* TX tail pointer / doorbell */
#define DMA_CHAN_RX_END_ADDR    0x1128u   /* RX tail pointer */
#define DMA_CHAN_TX_RING_LEN    0x112cu
#define DMA_CHAN_RX_RING_LEN    0x1130u
#define DMA_CHAN_INTR_ENA       0x1134u
#define DMA_CHAN_STATUS         0x1160u

#define DMA_CONTROL_ST          (1u << 0)
#define DMA_CONTROL_SR          (1u << 0)
#define DMA_CONTROL_OSP         (1u << 4)
#define DMA_TXPBL_SHIFT         16
#define DMA_RBSZ_SHIFT          1
#define DMA_RBSZ_MASK           0x3fffu   /* GENMASK(14,1) */

#define DMA_CHAN_STATUS_TI      (1u << 0)
#define DMA_CHAN_STATUS_TBU     (1u << 2)
#define DMA_CHAN_STATUS_RI      (1u << 6)
#define DMA_CHAN_STATUS_RBU     (1u << 7)
#define DMA_CHAN_STATUS_FBE     (1u << 12)
#define DMA_CHAN_STATUS_CDE     (1u << 13)
#define DMA_CHAN_STATUS_AIS     (1u << 14)
#define DMA_CHAN_STATUS_NIS     (1u << 15)

#define DMA_CHAN_INTR_ENA_NIE   (1u << 16)
#define DMA_CHAN_INTR_ENA_AIE   (1u << 15)
#define DMA_CHAN_INTR_ENA_CDE   (1u << 13)
#define DMA_CHAN_INTR_ENA_FBE   (1u << 12)
#define DMA_CHAN_INTR_ENA_RBUE  (1u << 7)
#define DMA_CHAN_INTR_ENA_RIE   (1u << 6)

/* DMA_DEBUG_STATUS_0: TX state in [3:0], RX state in [7:4]; 0 = stopped */
#define DMA_DBG_STATE_IDLE      0xf0u    /* both TS and RS fields zero */

/* Enhanced (16-byte) descriptor words */
struct net_dma_desc {
    volatile uint32_t d0;   /* read: buffer 1 address */
    volatile uint32_t d1;   /* read: buffer 2 address (unused) */
    volatile uint32_t d2;   /* TX read: buffer 1 size; RX write-back: RDES1 */
    volatile uint32_t d3;   /* read: OWN/FD/LD/...; write-back status */
};

#define TDES2_B1SIZE_MASK       0x3fffu
#define TDES3_PACKET_SIZE_MASK  0x7fffu
#define TDES3_LD                (1u << 28)
#define TDES3_FD                (1u << 29)
#define TDES3_IOC               (1u << 30)
#define TDES3_OWN               (1u << 31)

#define RDES3_PACKET_SIZE_MASK  0x7fffu
#define RDES3_ERROR_SUMMARY     (1u << 15)
#define RDES3_BUF1V             (1u << 24)
#define RDES3_IOC               (1u << 30)
#define RDES3_LD                (1u << 28)
#define RDES3_FD                (1u << 29)
#define RDES3_CTXT              (1u << 30)
#define RDES3_OWN               (1u << 31)

/* ------------------------------------------------------------------ */
/* Static pools (contract: no dynamic allocation)                     */
/* ------------------------------------------------------------------ */

#define N_DESC      NET_DESC_NUM
#define FRAME_SIZE  NET_FRAME_SIZE

/* DMA pools inside the NC window (see gmac1_profile.h for the layout
 * contract). const pointers keep every existing g_rx_desc[i] /
 * g_tx_pool[i][j] / &g_rx_desc[N_DESC-1] expression source-compatible
 * with the old static-array form. */
static struct net_dma_desc *const g_rx_desc =
    (struct net_dma_desc *)(NET_DMA_BASE + NET_DMA_RX_DESC_OFF);
static struct net_dma_desc *const g_tx_desc =
    (struct net_dma_desc *)(NET_DMA_BASE + NET_DMA_TX_DESC_OFF);
static uint8_t (*const g_rx_pool)[FRAME_SIZE] =
    (uint8_t (*)[FRAME_SIZE])(NET_DMA_BASE + NET_DMA_RX_POOL_OFF);
static uint8_t (*const g_tx_pool)[FRAME_SIZE] =
    (uint8_t (*)[FRAME_SIZE])(NET_DMA_BASE + NET_DMA_TX_POOL_OFF);

/* per-slot software state, indexed the same as the descriptor rings */
#define SLOT_FREE     0u   /* desc armed with OWN, empty (RX) / reusable (TX) */
#define SLOT_READY    1u   /* RX: frame waiting in the ready queue */
#define SLOT_PENDING  2u   /* TX: owned by DMA (or completed, not yet reaped) */

static uint8_t g_rx_state[N_DESC];
static uint8_t g_tx_state[N_DESC];
static uint16_t g_rx_len[N_DESC];

/* RX ready queue: lock-free SPSC ring of slot indices.
 * Single producer = the ISR (net_rx_reap), single consumer = the task
 * (NetRecv) - both run on CPU3, so there is no cross-core visibility
 * problem; head is written ONLY by the consumer, tail ONLY by the
 * producer, and no shared read-modify-write counter exists (a shared
 * g_rdy_cnt ++/-- raced once and corrupted the queue index, observed
 * as a slave data-abort hang 2026-09-27). Capacity N_DESC-1. */
static uint8_t g_rdy_q[N_DESC];
static volatile uint32_t g_rdy_head, g_rdy_tail;

/* cursor of the RX reap scan (next desc the ISR expects to be OWN) */
static uint32_t g_rx_cur;
/* TX allocation cursor: free slots are always a contiguous tail here */
static uint32_t g_tx_alloc;

struct NetDevice {
    uint32_t state;                     /* NET_STATE_* (slave-local view) */
    uint32_t port_id;                   /* NET_PORT_* this instance serves */
    uint32_t mode;                      /* NET_MODE_* (runtime, from config) */
    uint32_t autoneg;                   /* 1=autoneg, 0=forced link */
    uint32_t force_speed_mbps;          /* valid when autoneg==0 */
    uint32_t force_full_duplex;         /* valid when autoneg==0 */
    uint32_t promisc;                   /* 1=promiscuous rx filter */
    volatile struct net_ctrl *ctrl;  /* shared block @ 0x740ff000 */
    NetStats stats;
};

static struct NetDevice g_dev = {
    .state = NET_STATE_OFFLINE,
};

/* ------------------------------------------------------------------ */
/* Register helpers                                                   */
/* ------------------------------------------------------------------ */

static inline uint32_t net_reg_read(uint32_t off)
{
    return *(volatile uint32_t *)(GMAC1_BASE + off);
}

static inline void net_reg_write(uint32_t off, uint32_t val)
{
    *(volatile uint32_t *)(GMAC1_BASE + off) = val;
}

static inline void net_dsb(void)
{
    __asm__ volatile("dsb sy" ::: "memory");
}

/* ------------------------------------------------------------------ */
/* LOS interrupt API                                                  */
/*                                                                     */
/* ArchHwiCreate/ArchHwiDelete live in kernel/liteos_m/arch/arm/common/
 * los_common_interrupt.c; no public header declares them (the mica irq
 * backend calls them the same way). The gic-v3 port's createIrq applies
 * the per-core ownership contract for SPIs (Group1-NS, IROUTER to this
 * core, ICFGR from hwiMode, priority, enable) - no GICD work is needed
 * here, and the master never configures the slave's interrupts. */
/* ------------------------------------------------------------------ */

extern UINT32 ArchHwiCreate(HWI_HANDLE_T hwiNum, HWI_PRIOR_T hwiPrio,
                            HWI_MODE_T hwiMode, HWI_PROC_FUNC hwiHandler,
                            HwiIrqParam *irqParam);
extern UINT32 ArchHwiDelete(HWI_HANDLE_T hwiNum, HwiIrqParam *irqParam);

/* ------------------------------------------------------------------ */
/* MDIO (clause 22) write - for link forcing                           */
/*                                                                     */
/* The master owns the MDIO bus but stops writing it once probe() is   */
/* done (its 1s link work only READS). The slave only writes here,     */
/* during NetInit, well after the master probe finished - race-free. */
/* ------------------------------------------------------------------ */

#define GMAC1_MDIO_ADDR_REG     0x0200u
#define GMAC1_MDIO_DATA_REG     0x0204u
#define MDIO_BUSY               (1u << 0)
#define MDIO_GOC_WRITE          (1u << 2)
#define MDIO_GOC_READ           (3u << 2)
#define MDIO_CR_VAL             5u      /* CSR range for a 125MHz stmmaceth clock */
#define MDIO_PHY_SHIFT          21
#define MDIO_REG_SHIFT          16
#define MDIO_CR_SHIFT           8

/* BMCR (reg0) force encodings; autoneg bit (12) left cleared. */
#define BMCR_SPEED100           (1u << 13)
#define BMCR_FULLDPLX           (1u << 8)
/* BMCR (reg0) autoneg encodings (restore aneg after a forced config) */
#define BMCR_ANEG_ENABLE        (1u << 12)
#define BMCR_RESTART_ANEG       (1u << 9)

static int net_mdio_write(uint32_t phy, uint32_t reg, uint16_t val)
{
    uint32_t v;
    uint32_t poll;

    net_reg_write(GMAC1_MDIO_DATA_REG, val);
    v = MDIO_BUSY | MDIO_GOC_WRITE |
        (phy << MDIO_PHY_SHIFT) | (reg << MDIO_REG_SHIFT) |
        (MDIO_CR_VAL << MDIO_CR_SHIFT);
    net_reg_write(GMAC1_MDIO_ADDR_REG, v);

    for (poll = 0; poll < 100000u; poll++) {
        if ((net_reg_read(GMAC1_MDIO_ADDR_REG) & MDIO_BUSY) == 0) {
            return 0;
        }
    }
    return -1;
}

static int net_mdio_read(uint32_t phy, uint32_t reg, uint16_t *val)
{
    uint32_t v;
    uint32_t poll;

    v = MDIO_BUSY | MDIO_GOC_READ |
        (phy << MDIO_PHY_SHIFT) | (reg << MDIO_REG_SHIFT) |
        (MDIO_CR_VAL << MDIO_CR_SHIFT);
    net_reg_write(GMAC1_MDIO_DATA_REG, 0);
    net_reg_write(GMAC1_MDIO_ADDR_REG, v);

    for (poll = 0; poll < 100000u; poll++) {
        if ((net_reg_read(GMAC1_MDIO_ADDR_REG) & MDIO_BUSY) == 0) {
            *val = (uint16_t)(net_reg_read(GMAC1_MDIO_DATA_REG) & 0xffffu);
            return 0;
        }
    }
    return -1;
}

/* Force the PHY to a fixed speed/duplex (no autoneg). 1000BASE-T is
 * rejected: IEEE 802.3 requires autonegotiation for 1000M links.
 * Returns 0 on success. */
static int net_phy_force(uint32_t speed_mbps, uint32_t full_duplex)
{
    uint16_t bmcr = 0;
    uint32_t phy = GMAC1_PHY_ADDR;

    switch (speed_mbps) {
    case 10:
        break;                              /* speed bit clear */
    case 100:
        bmcr |= BMCR_SPEED100;
        break;
    default:
        return -1;                          /* incl. 1000: cannot force */
    }
    if (full_duplex != 0) {
        bmcr |= BMCR_FULLDPLX;
    }
    return net_mdio_write(phy, 0 /* BMCR */, bmcr);
}

/* ------------------------------------------------------------------ */
/* RX ring maintenance                                                */
/* ------------------------------------------------------------------ */

static void net_rx_arm(uint32_t slot)
{
    g_rx_desc[slot].d0 = (uint32_t)(uintptr_t)g_rx_pool[slot];
    g_rx_desc[slot].d1 = 0;
    g_rx_desc[slot].d2 = 0;
    net_dsb();
    g_rx_desc[slot].d3 = RDES3_OWN | RDES3_IOC | RDES3_BUF1V;
    g_rx_state[slot] = SLOT_FREE;
}

static void net_rx_kick(void)
{
    /* Full-ring tail: the DMA may walk the whole ring and wraps on its
     * own; descriptors still owned by software simply backpressure it. */
    net_dsb();
    net_reg_write(DMA_CHAN_RX_END_ADDR,
                 (uint32_t)(uintptr_t)&g_rx_desc[N_DESC - 1]);
}

/* Reap completed RX descriptors into the ready queue (bounded by ring
 * size). Called from the ISR and from NetRecv in polling mode. */
static void net_rx_reap(void)
{
    uint32_t processed = 0;

    while (processed < N_DESC) {
        uint32_t slot = g_rx_cur;
        uint32_t d3 = g_rx_desc[slot].d3;
        uint32_t queued = 0;

        if ((d3 & RDES3_OWN) != 0) {
            break;                       /* DMA still owns it: nothing new */
        }
        if (g_rx_state[slot] != SLOT_FREE) {
            break;                       /* wrap onto a queued frame */
        }

        if ((d3 & RDES3_CTXT) != 0) {
            /* context (timestamp/PTR) descriptor: no frame */
            g_dev.stats.rx_dropped++;
        } else if (((d3 & (RDES3_FD | RDES3_LD)) != (RDES3_FD | RDES3_LD)) ||
                   ((d3 & RDES3_ERROR_SUMMARY) != 0)) {
            /* split frame (RBSZ > MTU, unexpected) or errored frame */
            g_dev.stats.rx_errors++;
        } else {
            uint32_t len = d3 & RDES3_PACKET_SIZE_MASK;
            if ((len < 14u) || (len > FRAME_SIZE)) {
                g_dev.stats.rx_dropped++;
            } else if (((g_rdy_tail + 1u) % N_DESC) == g_rdy_head) {
                g_dev.stats.rx_overflow++;
                g_dev.stats.rx_dropped++;
            } else {
                /* keep the descriptor software-owned here: the frame data
                 * in g_rx_pool must survive until Recv() copies it out.
                 * Arming now would hand the buffer back to the DMA while
                 * the consumer may still read it (data-tearing window,
                 * fixed 2026-09-27). Recv() arms after the copy. */
                g_rx_len[slot] = (uint16_t)len;
                g_rx_state[slot] = SLOT_READY;
                g_rdy_q[g_rdy_tail] = (uint8_t)slot;
                g_rdy_tail = (g_rdy_tail + 1u) % N_DESC;
                queued = 1;
            }
        }

        if (queued == 0) {
            /* only recycle slots that did NOT enter the queue */
            net_rx_arm(slot);
        }
        g_rx_cur = (g_rx_cur + 1u) % N_DESC;
        processed++;
    }

    if (processed != 0) {
        net_rx_kick();                    /* release the window */
    }
}

/* ------------------------------------------------------------------ */
/* TX ring maintenance (lazy completion reaping, no TIE)              */
/* ------------------------------------------------------------------ */

/* Reap cursor: index of the oldest SLOT_PENDING descriptor. Kept
 * separate from g_tx_alloc (next free slot) because pending frames are
 * always *behind* the allocation cursor - starting the scan at g_tx_alloc
 * would stop on the first free slot and never reap anything. */
static uint32_t g_tx_reap;

static void net_tx_reap(void)
{
    while (g_tx_state[g_tx_reap] == SLOT_PENDING) {
        if ((g_tx_desc[g_tx_reap].d3 & TDES3_OWN) != 0) {
            break;                       /* DMA still owns it */
        }
        if ((g_tx_desc[g_tx_reap].d3 & RDES3_ERROR_SUMMARY) != 0) {
            g_dev.stats.tx_errors++;
        }
        g_dev.stats.tx_completed++;
        g_tx_state[g_tx_reap] = SLOT_FREE;
        g_tx_reap = (g_tx_reap + 1u) % N_DESC;
    }
}

/* ------------------------------------------------------------------ */
/* Interrupt service (interrupt mode)                                 */
/* ------------------------------------------------------------------ */

static VOID NetIsr(VOID)
{
    uint32_t status = net_reg_read(DMA_CHAN_STATUS);
    uint32_t clr = status & (DMA_CHAN_STATUS_RI | DMA_CHAN_STATUS_RBU |
                             DMA_CHAN_STATUS_FBE | DMA_CHAN_STATUS_CDE |
                             DMA_CHAN_STATUS_AIS | DMA_CHAN_STATUS_NIS);

    /* W1C on the bits we handle */
    net_reg_write(DMA_CHAN_STATUS, clr);

    g_dev.stats.irq_count++;

    if ((status & (DMA_CHAN_STATUS_RI | DMA_CHAN_STATUS_RBU)) != 0) {
        net_rx_reap();
    }
    if ((status & DMA_CHAN_STATUS_AIS) != 0) {
        if ((status & (DMA_CHAN_STATUS_FBE | DMA_CHAN_STATUS_CDE)) != 0) {
            g_dev.stats.dma_errors++;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Contract implementation                                            */
/* ------------------------------------------------------------------ */

int NetInit(const NetConfig *cfg, NetDevice **dev)
{
    uint32_t val, poll;
    static const uint8_t mac[6] = GMAC1_DEFAULT_MAC;

    if ((cfg == NULL) || (dev == NULL)) {
        return NET_EINVAL;
    }
    if (cfg->port_id != NET_PORT_GMAC1_RK3568) {
        return NET_ENODEV;           /* unknown/not handed-over port */
    }
    if (cfg->mode > NET_MODE_HYBRID) {
        return NET_EINVAL;
    }
    if ((cfg->autoneg == 0) &&
        ((cfg->speed_mbps != 10) && (cfg->speed_mbps != 100))) {
        /* 1000BASE-T cannot be force-linked (IEEE 802.3 mandates autoneg) */
        return NET_EINVAL;
    }
    if (g_dev.state == NET_STATE_RUNNING) {
        return NET_EBUSY;           /* running: Stop first */
    }
    /* OFFLINE or READY both reach here. READY = reconfigure with a new
     * NetConfig (e.g. switch mode or link policy): Init is a full
     * reprogram anyway - DMA soft reset + every register rewritten - so
     * re-entry from READY is side-effect free. */

    /* --- link policy: force the PHY before touching the MAC --- *
     * The master stops writing MDIO after probe() (its link work only
     * reads), so this write is race-free. */
    if (cfg->autoneg == 0) {
        if (net_phy_force(cfg->speed_mbps, cfg->full_duplex) != 0) {
            PRINT_ERR("[net] PHY force %uM failed\n", cfg->speed_mbps);
            return NET_EIO;
        }
    } else {
        /* Restore autoneg ONLY if a previous Init forced the PHY (BMCR aneg
         * bit cleared); an already-autonegotiating PHY must not be disturbed
         * - writing a restart would needlessly bounce the link at every
         * re-Init. The advertisement register is never modified by this
         * driver, so the power-on default abilities stay in effect. */
        uint16_t bmcr = 0;
        if (net_mdio_read(GMAC1_PHY_ADDR, 0 /* BMCR */, &bmcr) != 0) {
            PRINT_ERR("[net] PHY BMCR read failed\n");
            return NET_EIO;
        }
        if ((bmcr & BMCR_ANEG_ENABLE) == 0) {
            if (net_mdio_write(GMAC1_PHY_ADDR, 0 /* BMCR */,
                              BMCR_ANEG_ENABLE | BMCR_RESTART_ANEG) != 0) {
                PRINT_ERR("[net] PHY autoneg restore failed\n");
                return NET_EIO;
            }
        }
    }

    /* --- DMA soft reset (bus + channel state machines) --- */
    net_reg_write(DMA_BUS_MODE, net_reg_read(DMA_BUS_MODE) | DMA_BUS_MODE_SFT_RESET);
    for (poll = 0; poll < 100000u; poll++) {
        if ((net_reg_read(DMA_BUS_MODE) & DMA_BUS_MODE_SFT_RESET) == 0) {
            break;
        }
    }
    if (poll == 100000u) {
        PRINT_ERR("[net] DMA soft reset timeout\n");
        return NET_EIO;
    }

    /* --- AXI bus mode: 32-bit addressing, fixed burst, short bursts --- */
    net_reg_write(DMA_SYS_BUS_MODE,
                 DMA_SYS_BUS_MODE_FB | DMA_SYS_BUS_BLEN4 | DMA_SYS_BUS_BLEN8 |
                 DMA_SYS_BUS_BLEN16 | DMA_SYS_BUS_BLEN32);

    /* --- MAC static config: GMII mode (speed via PHY/RGMII), rx filter
     *     per config: promiscuous (sniffer/ECAT) or DA0+multicast --- */
    val = net_reg_read(GMAC_CONFIG);
    val &= ~(GMAC_CONFIG_PS | GMAC_CONFIG_FES | GMAC_CONFIG_TE | GMAC_CONFIG_RE);
    val |= GMAC_CONFIG_DM | GMAC_CONFIG_JD | GMAC_CONFIG_BE |
           GMAC_CONFIG_DCRS;
    net_reg_write(GMAC_CONFIG, val);

    if (cfg->promisc != 0) {
        net_reg_write(GMAC_PACKET_FILTER,
                     GMAC_PACKET_FILTER_PR | GMAC_PACKET_FILTER_PM |
                     GMAC_PACKET_FILTER_HMC | GMAC_PACKET_FILTER_PCF);
    } else {
        /* unicast -> GMAC_ADDR0 filter, multicast pass-all, control in */
        net_reg_write(GMAC_PACKET_FILTER,
                     GMAC_PACKET_FILTER_PM | GMAC_PACKET_FILTER_HMC |
                     GMAC_PACKET_FILTER_PCF);
    }

    net_reg_write(GMAC_ADDR0_HIGH, ((uint32_t)mac[5] << 8) | mac[4] |
                  (1u << 31));  /* always valid */
    net_reg_write(GMAC_ADDR0_LOW, ((uint32_t)mac[3] << 24) | ((uint32_t)mac[2] << 16) |
                  ((uint32_t)mac[1] << 8) | mac[0]);

    /* --- MTL: single queue, store-and-forward both ways --- */
    net_reg_write(MTL_OPERATION_MODE, MTL_OP_SCHALG_SP | MTL_OP_RAA_WSP);
    net_reg_write(MTL_RXQ_DMA_MAP0, 0);   /* RX queue 0 -> DMA channel 0 */

    val = net_reg_read(MTL_TXQ0_OP_MODE);
    val |= MTL_OP_MODE_TXQEN | MTL_OP_MODE_TSF | MTL_OP_MODE_TTC_32;
    net_reg_write(MTL_TXQ0_OP_MODE, val);

    val = net_reg_read(MTL_RXQ0_OP_MODE);
    val |= MTL_OP_MODE_RSF | MTL_OP_MODE_RTC_64;
    net_reg_write(MTL_RXQ0_OP_MODE, val);

    /* --- DMA channel 0: rings, buffer size, engines still stopped --- */
    net_reg_write(DMA_CHAN_TX_BASE, (uint32_t)(uintptr_t)g_tx_desc);
    net_reg_write(DMA_CHAN_RX_BASE, (uint32_t)(uintptr_t)g_rx_desc);
    net_reg_write(DMA_CHAN_TX_RING_LEN, N_DESC - 1);
    net_reg_write(DMA_CHAN_RX_RING_LEN, N_DESC - 1);

    net_reg_write(DMA_CHAN_TX_CONTROL,
                 (GMAC1_DMA_PBL << DMA_TXPBL_SHIFT) | DMA_CONTROL_OSP);
    net_reg_write(DMA_CHAN_RX_CONTROL,
                 ((FRAME_SIZE << DMA_RBSZ_SHIFT) & DMA_RBSZ_MASK));

    /* --- build the RX ring, wipe the TX ring --- */
    for (uint32_t i = 0; i < N_DESC; i++) {
        net_rx_arm(i);
        g_tx_desc[i].d0 = 0;
        g_tx_desc[i].d1 = 0;
        g_tx_desc[i].d2 = 0;
        g_tx_desc[i].d3 = 0;
        g_tx_state[i] = SLOT_FREE;
    }
    g_rx_cur = 0;
    g_tx_alloc = 0;
    g_tx_reap = 0;
    g_rdy_head = 0;
    g_rdy_tail = 0;

    g_dev.port_id = cfg->port_id;
    g_dev.mode = cfg->mode;
    g_dev.autoneg = (cfg->autoneg != 0) ? 1u : 0u;
    g_dev.force_speed_mbps = (cfg->autoneg == 0) ? cfg->speed_mbps : 0;
    g_dev.force_full_duplex = (cfg->autoneg == 0) ? ((cfg->full_duplex != 0) ? 1u : 0u) : 0;
    g_dev.promisc = (cfg->promisc != 0) ? 1u : 0u;
    g_dev.ctrl = (volatile struct net_ctrl *)NET_CTRL_PHYS;
    g_dev.state = NET_STATE_READY;

    if ((g_dev.ctrl->magic == NET_CTRL_MAGIC) &&
        (g_dev.ctrl->version == NET_CTRL_VERSION)) {
        g_dev.ctrl->slave_state = NET_STATE_READY;
    }

    *dev = &g_dev;
    return 0;
}

int NetStart(NetDevice *dev)
{
    if ((dev == NULL) || (dev != &g_dev)) {
        return NET_EINVAL;
    }
    if (dev->state == NET_STATE_RUNNING) {
        return 0;                        /* idempotent */
    }
    if (dev->state != NET_STATE_READY) {
        return NET_EIO;               /* FAULT: needs Stop first */
    }

    if (dev->mode != NET_MODE_POLL) {
        if (ArchHwiCreate((HWI_HANDLE_T)GMAC1_IRQ_INTID, GMAC1_IRQ_LOS_PRIO,
                          HWI_MODE_TRIGGER_LEVEL,
                          (HWI_PROC_FUNC)NetIsr, (HwiIrqParam *)0) != LOS_OK) {
            PRINT_ERR("[net] ArchHwiCreate(%d) failed\n", GMAC1_IRQ_INTID);
            return NET_EIO;
        }
    }

    /* clear any latched status, arm interrupts (masked out in poll mode:
     * the MAC stays silent on the wire and Recv() drives everything) */
    net_reg_write(DMA_CHAN_STATUS, 0xffffffffu);
    if (dev->mode != NET_MODE_POLL) {
        net_reg_write(DMA_CHAN_INTR_ENA,
                     DMA_CHAN_INTR_ENA_NIE | DMA_CHAN_INTR_ENA_AIE |
                     DMA_CHAN_INTR_ENA_RIE | DMA_CHAN_INTR_ENA_RBUE |
                     DMA_CHAN_INTR_ENA_FBE | DMA_CHAN_INTR_ENA_CDE);
    } else {
        net_reg_write(DMA_CHAN_INTR_ENA, 0);
    }

    /* engine bring-up order (Linux dwmac4): tail ptr, then DMA SR/ST,
     * then MAC RE/TE */
    net_rx_kick();
    net_reg_write(DMA_CHAN_RX_CONTROL,
                 net_reg_read(DMA_CHAN_RX_CONTROL) | DMA_CONTROL_SR);
    net_reg_write(GMAC_CONFIG, net_reg_read(GMAC_CONFIG) | GMAC_CONFIG_RE);

    /* TX tail at base before starting the engine (Linux dwmac4 does the
     * same in init/open): with the open-interval [cursor, tail) fetch
     * semantics this arms an empty ring; every NetSend doorbell then
     * points at the next free slot and advances the window. */
    net_reg_write(DMA_CHAN_TX_END_ADDR, (uint32_t)(uintptr_t)g_tx_desc);
    net_reg_write(DMA_CHAN_TX_CONTROL,
                 net_reg_read(DMA_CHAN_TX_CONTROL) | DMA_CONTROL_ST);
    net_reg_write(GMAC_CONFIG, net_reg_read(GMAC_CONFIG) | GMAC_CONFIG_TE);

    dev->state = NET_STATE_RUNNING;
    if ((dev->ctrl->magic == NET_CTRL_MAGIC) &&
        (dev->ctrl->version == NET_CTRL_VERSION)) {
        dev->ctrl->slave_state = NET_STATE_RUNNING;
    }
    return 0;
}

int NetStop(NetDevice *dev, uint32_t timeout_us)
{
    if ((dev == NULL) || (dev != &g_dev)) {
        return NET_EINVAL;
    }
    if (dev->state != NET_STATE_RUNNING) {
        return 0;
    }

    /* mask everything first so the ISR cannot restart engines */
    net_reg_write(DMA_CHAN_INTR_ENA, 0);
    net_reg_write(DMA_CHAN_STATUS, 0xffffffffu);

    net_reg_write(DMA_CHAN_TX_CONTROL,
                 net_reg_read(DMA_CHAN_TX_CONTROL) & ~DMA_CONTROL_ST);
    net_reg_write(DMA_CHAN_RX_CONTROL,
                 net_reg_read(DMA_CHAN_RX_CONTROL) & ~DMA_CONTROL_SR);
    net_reg_write(GMAC_CONFIG,
                 net_reg_read(GMAC_CONFIG) & ~(GMAC_CONFIG_TE | GMAC_CONFIG_RE));

    /* bounded quiesce: DMA_DEBUG_STATUS_0 TS[3:0]/RS[7:4] both 0 */
    {
        /* ~30 loop iterations per microsecond on a ~1.5GHz A55 */
        uint32_t budget = (timeout_us == 0) ? 1u : (timeout_us * 30u);
        uint32_t quiet = 0;
        while (budget-- != 0) {
            uint32_t dbg = net_reg_read(DMA_DEBUG_STATUS_0);
            if (((dbg & 0xfu) == 0) && (((dbg >> 4) & 0xfu) == 0)) {
                quiet = 1;
                break;
            }
        }
        if (quiet == 0) {
            dev->state = NET_STATE_FAULT;
            if ((dev->ctrl->magic == NET_CTRL_MAGIC) &&
                (dev->ctrl->version == NET_CTRL_VERSION)) {
                dev->ctrl->slave_state = NET_STATE_FAULT;
            }
            PRINT_ERR("[net] Stop: DMA quiesce timeout\n");
            return NET_ETIMEDOUT;
        }
    }

    if (dev->mode != NET_MODE_POLL) {
        /* ArchHwiDelete masks the SPI at the GICD (per-core ownership) */
        (VOID)ArchHwiDelete((HWI_HANDLE_T)GMAC1_IRQ_INTID, (HwiIrqParam *)0);
    }

    /* reset the rings for the next Start */
    for (uint32_t i = 0; i < N_DESC; i++) {
        net_rx_arm(i);
        g_tx_desc[i].d3 = 0;
        g_tx_state[i] = SLOT_FREE;
    }
    g_rx_cur = 0;
    g_tx_alloc = 0;
    g_tx_reap = 0;
    g_rdy_head = 0;
    g_rdy_tail = 0;

    dev->state = NET_STATE_READY;
    if ((dev->ctrl->magic == NET_CTRL_MAGIC) &&
        (dev->ctrl->version == NET_CTRL_VERSION)) {
        dev->ctrl->slave_state = NET_STATE_READY;
    }
    return 0;
}

int NetSend(NetDevice *dev, const uint8_t *frame, uint32_t len)
{
    uint32_t slot;

    if ((dev == NULL) || (dev != &g_dev) || (frame == NULL)) {
        return NET_EINVAL;
    }
    if (dev->state != NET_STATE_RUNNING) {
        return NET_EIO;
    }
    if ((len < 14u) || (len > 1514u)) {
        return NET_EMSGSIZE;
    }

    net_tx_reap();

    /* find a free slot: with single-producer allocation the free slots
     * form a contiguous run starting at g_tx_alloc */
    for (slot = 0; slot < N_DESC; slot++) {
        uint32_t idx = (g_tx_alloc + slot) % N_DESC;
        if (g_tx_state[idx] == SLOT_FREE) {
            g_tx_alloc = idx;
            break;
        }
    }
    if (slot == N_DESC) {
        dev->stats.tx_busy++;
        return NET_EAGAIN;
    }

    memcpy(g_tx_pool[g_tx_alloc], frame, len);

    g_tx_desc[g_tx_alloc].d0 = (uint32_t)(uintptr_t)g_tx_pool[g_tx_alloc];
    g_tx_desc[g_tx_alloc].d1 = 0;
    g_tx_desc[g_tx_alloc].d2 = len & TDES2_B1SIZE_MASK;
    net_dsb();
    g_tx_desc[g_tx_alloc].d3 = TDES3_OWN | TDES3_FD | TDES3_LD |
                               (len & TDES3_PACKET_SIZE_MASK);
    g_tx_state[g_tx_alloc] = SLOT_PENDING;

    /* Doorbell semantics (dwmac4): the DMA fetches descriptors in
     * [cursor, tail) - the descriptor the tail points at is NOT fetched.
     * Linux therefore writes the NEXT free slot (stmmac_main.c xmit:
     * tail = dma_tx_phy + cur_tx * desc_size, after cur_tx advanced).
     * Pointing the tail at the descriptor we just armed makes the DMA
     * skip it entirely (OWN never cleared, TBU latched; observed
     * 2026-09-23). */
    g_tx_alloc = (g_tx_alloc + 1u) % N_DESC;
    net_reg_write(DMA_CHAN_TX_END_ADDR,
                 (uint32_t)(uintptr_t)&g_tx_desc[g_tx_alloc]);

    dev->stats.tx_accepted++;
    return 0;
}

int NetRecv(NetDevice *dev, uint8_t *frame, uint32_t capacity, uint32_t *len)
{
    uint32_t slot, flen;

    if ((dev == NULL) || (dev != &g_dev) || (frame == NULL) || (len == NULL)) {
        return NET_EINVAL;
    }
    if (dev->state != NET_STATE_RUNNING) {
        return NET_EIO;
    }

    /* POLL: Recv() is the only reaper. HYBRID: the ISR reaps ahead of us;
     * reaping here too is idempotent and backstops a dropped IRQ. */
    if (dev->mode != NET_MODE_IRQ) {
        net_rx_reap();
    }

    while (g_rdy_head != g_rdy_tail) {
        slot = g_rdy_q[g_rdy_head];
        /* NOTE: g_rx_state[slot] is NOT checked here - the slot was armed
         * back to SLOT_FREE right after being queued (the frame data lives
         * in g_rx_pool until Recv copies it out). The SPSC ring itself
         * guarantees the index/len pair is consistent. */
        flen = g_rx_len[slot];
        if (flen > capacity) {
            /* contract: short buffer -> drop the frame and count it */
            g_rdy_head = (g_rdy_head + 1u) % N_DESC;
            net_rx_arm(slot);
            net_rx_kick();
            dev->stats.rx_dropped++;
            return NET_EMSGSIZE;
        }

        memcpy(frame, g_rx_pool[slot], flen);

        g_rdy_head = (g_rdy_head + 1u) % N_DESC;
        net_rx_arm(slot);
        net_rx_kick();

        dev->stats.rx_delivered++;
        *len = flen;
        return 0;
    }
    return NET_EAGAIN;
}

int NetGetLinkInfo(NetDevice *dev, NetLinkInfo *info)
{
    if ((dev == NULL) || (dev != &g_dev) || (info == NULL)) {
        return NET_EINVAL;
    }

    if ((dev->ctrl->magic != NET_CTRL_MAGIC) ||
        (dev->ctrl->version != NET_CTRL_VERSION)) {
        memset(info, 0, sizeof(*info));
        return 0;                        /* master not up: invalid snapshot */
    }

    /* the 64-bit timestamp is written by another core: double-read valid */
    do {
        info->valid = dev->ctrl->valid;
        info->link_up = dev->ctrl->link_up;
        info->speed_mbps = dev->ctrl->speed_mbps;
        info->full_duplex = dev->ctrl->full_duplex;
        info->sample_time_ns = dev->ctrl->sample_time_ns;
    } while (info->valid != dev->ctrl->valid);

    /* forced link: the master's aneg-based speed parse is meaningless
     * (no aneg result registers to read) - publish the forced policy */
    if (dev->autoneg == 0) {
        info->speed_mbps = dev->force_speed_mbps;
        info->full_duplex = dev->force_full_duplex;
    }
    return 0;
}

int NetGetStats(NetDevice *dev, NetStats *stats)
{
    if ((dev == NULL) || (dev != &g_dev) || (stats == NULL)) {
        return NET_EINVAL;
    }

    net_tx_reap();                        /* fold in lazy TX completions */
    *stats = dev->stats;
    if ((dev->ctrl->magic == NET_CTRL_MAGIC) &&
        (dev->ctrl->version == NET_CTRL_VERSION)) {
        stats->link_changes = dev->ctrl->link_changes;
    }
    return 0;
}
