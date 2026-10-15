/*
 * Copyright (c) 2026 morpheus
 *
 * can1_profile.h - board profile for the RK3568 CAN1 real-time endpoint.
 *
 * Everything board-specific lives here; porting to another CAN controller
 * means a new profile file + master-side dts node. The public port
 * selector macro lives in can.h (CAN_PORT_CAN1_RK3568 == 1 == this
 * profile).
 *
 * Hardware contract (set up by the master mcs_can.ko before the slave
 * touches CAN1): baudclk retargeted to 100MHz + both clocks held on,
 * CRU resets pulsed, can1m1 pinmux applied. The true read-back baudclk
 * rate is published in the shared control block (can_ctrl.h) and the
 * bit-timing solver consumes THAT value. The interrupt is owned by the
 * slave: CanStart registers INTID 34 via ArchHwiCreate, whose gic-v3
 * createIrq applies the per-core ownership contract (Group1-NS, IROUTER
 * to CPU3, trigger, priority, enable); the master never touches it.
 */

#ifndef _CAN1_PROFILE_H_
#define _CAN1_PROFILE_H_

#include <stdint.h>
#include "can.h"    /* CanBittiming: public timing contract */

#ifdef __cplusplus
extern "C" {
#endif

/* Rockchip CAN 2.0 controller (rk356x-oh.dtsi can@fe580000) */
#define CAN1_BASE               0xFE580000UL
#define CAN1_REG_SIZE           0x1000UL

/* shared control block (one 0x100 page per port in the MCS window) */
#define CAN1_CTRL_PHYS          0x740FF100UL

/* GIC SPI 2 -> INTID 34, level-triggered; owned by the slave core (the
 * gic-v3 createIrq routes the SPI to the creating core's affinity). */
#define CAN1_IRQ_INTID          34
/* LOS prio 0..31 (0 = highest); the gic-v3 port maps it to the 8-bit GIC
 * field as prio << 3 (same value the net port uses, 4 -> 0x20). */
#define CAN1_IRQ_LOS_PRIO       4

/* Hardware fifo geometry (rk356x-oh.dtsi):
 *   tx-fifo-depth = 1  -> CanSend is a synchronous single-frame handshake
 *   rx-fifo-depth = 6  -> the ISR/Recv drain empties up to 6 frames/pass
 */
#define CAN1_RX_FIFO_DEPTH      6

/* Default bitrate when CanConfig.bitrate == 0 and no raw timing given. */
#define CAN1_DEFAULT_BITRATE    500000u

#ifdef __cplusplus
}
#endif

#endif /* _CAN1_PROFILE_H_ */
