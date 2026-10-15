/*
 * Copyright (c) 2026 morpheus
 *
 * can.h - generic real-time CAN (classic CAN 2.0B) contract for the AMP
 * slave core.
 *
 * Protocol-agnostic frame-level driver in the same style as net.h: any
 * higher-layer CAN protocol (J1939, CANopen, proprietary) runs on top of
 * CanSend/CanRecv. CanDevice is opaque and owned by the board driver
 * (can_rk3568.c).
 *
 * Frame contract: classic CAN 2.0B only - standard (11-bit) and extended
 * (29-bit) ids, data and remote frames, 0..8 payload bytes. CAN FD is
 * NOT supported (the RK3568 CAN IP is used in its CAN 2.0 mode).
 *
 * Division of labor: the Linux master (mcs_can.ko) owns clocks (baudclk
 * retargeted to 100MHz and held on), CRU resets, the pinmux and
 * publishes the platform facts into the per-port shared control block
 * (can_ctrl.h; one 0x100 page per handed-over controller); this driver
 * owns the whole data path: operating mode, bit timing, acceptance
 * filters, the rx fifo, the single-frame tx fifo handshake and the CAN
 * interrupt (registered via ArchHwiCreate - per-core GICD ownership,
 * the master never touches it).
 *
 * Bit timing: two config styles in CanConfig.
 *   - bitrate (default): the driver solves the prescaler/tseg values for
 *     a ~87.5% sample point against the READ-BACK baudclk published by
 *     the master. 800kbps is NOT reachable at a 100MHz bit clock
 *     (100M/800k = 125, the prescaler steps by 2 - no even solution).
 *   - raw brp/sjw/ts1/ts2 (use_raw_timing = 1): expert escape hatch,
 *     values are written straight into NBTP.
 *
 * Transmit model: the RK3568 CAN tx fifo is ONE frame deep; CanSend is
 * synchronous - it queues the frame, completes the hardware handshake and
 * waits (bounded by timeout_us) for the transmission to finish before
 * returning. Send never queues behind a busy transmitter.
 *
 * Receive model: CAN_MODE_IRQ delivers frames through a lock-free ready
 * queue filled by the ISR; CAN_MODE_POLL never enables the interrupt and
 * CanRecv reaps the hardware fifo directly; CAN_MODE_HYBRID does both
 * (the ISR reaps ahead, Recv reaps again as a backstop).
 */

#ifndef _CAN_H_
#define _CAN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CanDevice CanDevice;

/* ------------------------------------------------------------------ */
/* Port selection: one id per CAN controller the master handed over.  */
/* The driver holds one CanDevice instance per port - both CAN1 and   */
/* CAN2 can run CONCURRENTLY (independent fifos, irqs, ctrl blocks). */
/* ------------------------------------------------------------------ */
#define CAN_PORT_CAN1_RK3568    1    /* RK3568 CAN1 (fe580000, can1m1 pins) */
#define CAN_PORT_CAN2_RK3568    2    /* RK3568 CAN2 (fe590000, can2m0 pins) */

/* ------------------------------------------------------------------ */
/* Rx operating mode (CanConfig.mode)                                  */
/* ------------------------------------------------------------------ */
#define CAN_MODE_IRQ            0    /* interrupt driven (default) */
#define CAN_MODE_POLL           1    /* pure polling: no IRQ, Recv() reaps */
#define CAN_MODE_HYBRID         2    /* IRQ driven + Recv() reaps as backstop */

/* ------------------------------------------------------------------ */
/* Bus operating mode (CanConfig.work_mode)                            */
/* ------------------------------------------------------------------ */
#define CAN_WORK_NORMAL         0    /* normal bus participation (default) */
#define CAN_WORK_LOOPBACK       1    /* internal loopback, self-ack, nothing
                                        driven on the pins: the zero-
                                        hardware bring-up / unit-test mode */
#define CAN_WORK_SILENT         2    /* listen-only: receive only, no ack,
                                        no transmit (bus monitoring) */
#define CAN_WORK_SILENT_LB      3    /* loopback + listen-only: internal
                                        loopback, nothing on the pins */

/* ------------------------------------------------------------------ */
/* Frame                                                               */
/* ------------------------------------------------------------------ */
#define CAN_FRAME_EFF           (1u << 0)  /* 29-bit id (else 11-bit) */
#define CAN_FRAME_RTR           (1u << 1)  /* remote transmission request */

typedef struct {
    uint32_t id;            /* 11-bit (0x0..0x7ff) or 29-bit with CAN_FRAME_EFF */
    uint8_t  flags;         /* CAN_FRAME_EFF | CAN_FRAME_RTR */
    uint8_t  dlc;           /* data length code 0..8 (payload bytes) */
    uint8_t  reserved;
    uint8_t  data[8];
} CanFrame;

/* ------------------------------------------------------------------ */
/* Bit timing (raw style)                                              */
/* ------------------------------------------------------------------ */
typedef struct {
    uint32_t brp;           /* prescaler, EVEN, 2..512 (clock tq = baudclk/brp) */
    uint32_t sjw;           /* resync jump width, 1..128 tq */
    uint32_t ts1;           /* prop+phase1, 1..128 tq (sample point = (1+ts1)/N) */
    uint32_t ts2;           /* phase2, 1..128 tq; N = 1+ts1+ts2 tq per bit */
} CanBittiming;

/* ------------------------------------------------------------------ */
/* Init-time configuration                                            */
/* ------------------------------------------------------------------ */
typedef struct {
    uint32_t port_id;       /* CAN_PORT_*: which handed-over controller */
    uint32_t mode;          /* CAN_MODE_* (default IRQ when 0) */
    uint32_t work_mode;     /* CAN_WORK_* (default NORMAL when 0) */
    /* Bit timing: raw wins when use_raw_timing != 0, else bitrate. */
    uint32_t use_raw_timing;/* 0 = solve from bitrate (default) */
    uint32_t bitrate;       /* bits/s; default 500000 when both styles are 0 */
    CanBittiming timing;    /* raw values (use_raw_timing = 1) */
    /* Acceptance filter (single code/mask bank on the 29-bit id).
     * filter_en = 0 (default): accept everything.
     * filter_en = 1: accept id where (id & ~filter_mask) == (filter_code
     * & ~filter_mask); filter_mask = 0 receives exactly filter_code. */
    uint32_t filter_en;
    uint32_t filter_code;
    uint32_t filter_mask;
} CanConfig;

/* ------------------------------------------------------------------ */
/* Runtime information                                                */
/* ------------------------------------------------------------------ */
#define CAN_BUS_STOPPED         0   /* driver not started (reset mode) */
#define CAN_BUS_ERROR_ACTIVE    1   /* rx/tx error counters < 96 */
#define CAN_BUS_ERROR_WARNING   2   /* error counter >= 96 */
#define CAN_BUS_ERROR_PASSIVE   3   /* error counter >= 128 */
#define CAN_BUS_BUS_OFF         4   /* controller off the bus */

typedef struct {
    uint32_t state;         /* CAN_BUS_* */
    uint32_t rx_err_cnt;    /* hardware RX error counter 0..255 */
    uint32_t tx_err_cnt;    /* hardware TX error counter 0..255 */
    uint32_t bus_off_count; /* bus-off events observed since Init */
} CanBusInfo;

typedef struct {
    uint32_t tx_accepted;   /* Send() calls accepted */
    uint32_t tx_completed;  /* transmissions confirmed done */
    uint32_t tx_errors;     /* TX failure indicators */
    uint32_t tx_timeouts;   /* Send() rejected with -ETIMEDOUT */
    uint32_t rx_delivered;  /* frames copied out via Recv() */
    uint32_t rx_dropped;    /* malformed / filtered-out / queue-full */
    uint32_t rx_errors;     /* RX error indicators */
    uint32_t rx_overflow;   /* ready queue full at ISR time */
    uint32_t rx_dup_suppressed; /* loopback self-frame duplicate swallowed */
    uint32_t irq_count;     /* INTID deliveries (interrupt modes) */
    uint32_t bus_off_count; /* mirror of CanBusInfo.bus_off_count */
    uint32_t reserved[3];
} CanStats;

/* POSIX errno values (the bare slave has no errno.h) */
#define CAN_EAGAIN     (-11)
#define CAN_EINVAL     (-22)
#define CAN_EMSGSIZE   (-90)
#define CAN_ENODEV     (-19)
#define CAN_EBUSY      (-16)
#define CAN_EIO        (-5)
#define CAN_ETIMEDOUT  (-110)
#define CAN_ENOTSUP    (-95)

int CanInit(const CanConfig *cfg, CanDevice **dev);
int CanStart(CanDevice *dev);
int CanStop(CanDevice *dev, uint32_t timeout_us);
int CanSend(CanDevice *dev, const CanFrame *frame, uint32_t timeout_us);
int CanRecv(CanDevice *dev, CanFrame *frame);
int CanGetBusState(CanDevice *dev, CanBusInfo *info);
int CanGetStats(CanDevice *dev, CanStats *stats);

#ifdef __cplusplus
}
#endif

#endif /* _CAN_H_ */
