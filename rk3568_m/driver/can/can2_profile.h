/*
 * Copyright (c) 2026 morpheus
 *
 * can2_profile.h - board profile for the RK3568 CAN2 real-time endpoint.
 *
 * Twin of can1_profile.h: same CAN 2.0 IP, different base/irq/ctrl page.
 * Platform facts (verified against the OH kernel rk356x-oh.dtsi):
 *   can@fe590000, interrupts = GIC_SPI 3 (INTID 35), CLK_CAN2/PCLK_CAN2,
 *   SRST_CAN2/SRST_P_CAN2, tx-fifo-depth 1, rx-fifo-depth 6.
 * Pin groups: can2m0 = GPIO4_B4 (rx) / GPIO4_B5 (tx) func3;
 *             can2m1 = GPIO2_B1 (rx) / GPIO2_B2 (tx) func4.
 * The master dts node picks the group (default can2m0); the pins only
 * matter for a real external bus - internal loopback never leaves the
 * die (a pinmux failure on the master side is non-fatal by design).
 *
 * Handover contract (set up by the master mcs_can.ko before the slave
 * touches CAN2): baudclk retargeted to 100MHz + both clocks held on,
 * CRU resets pulsed, can2m0 pinmux applied. The true read-back baudclk
 * rate is published in the shared control block (can_ctrl.h) and the
 * bit-timing solver consumes THAT value. The interrupt is owned by the
 * slave: CanStart registers INTID 35 via ArchHwiCreate, whose gic-v3
 * createIrq applies the per-core ownership contract (Group1-NS, IROUTER
 * to CPU3, trigger, priority, enable); the master never touches it.
 */

#ifndef _CAN2_PROFILE_H_
#define _CAN2_PROFILE_H_

#include <stdint.h>
#include "can.h"    /* CanBittiming: public timing contract */

#ifdef __cplusplus
extern "C" {
#endif

/* Rockchip CAN 2.0 controller (rk356x-oh.dtsi can@fe590000) */
#define CAN2_BASE               0xFE590000UL
#define CAN2_REG_SIZE           0x1000UL

/* shared control block (one 0x100 page per port in the MCS window) */
#define CAN2_CTRL_PHYS          0x740FF200UL

/* GIC SPI 3 -> INTID 35, level-triggered; owned by the slave core */
#define CAN2_IRQ_INTID          35
/* LOS prio 0..31 (0 = highest); same mapping as CAN1 (4 -> 0x20) */
#define CAN2_IRQ_LOS_PRIO       4

/* Hardware fifo geometry (rk356x-oh.dtsi, same IP as CAN1):
 *   tx-fifo-depth = 1  -> CanSend is a synchronous single-frame handshake
 *   rx-fifo-depth = 6  -> the ISR/Recv drain empties up to 6 frames/pass
 */
#define CAN2_RX_FIFO_DEPTH      6

/* Default bitrate when CanConfig.bitrate == 0 and no raw timing given. */
#define CAN2_DEFAULT_BITRATE    500000u

#ifdef __cplusplus
}
#endif

#endif /* _CAN2_PROFILE_H_ */
