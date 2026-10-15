/* SPDX-License-Identifier: GPL-2.0 */
/*
 * can_ctrl.h - shared control block between the Linux master (mcs_can.ko)
 * and the LiteOS-M slave (can_rk3568.c) for the RK3568 real-time CAN
 * ports. Slave-side copy; the master copy in
 * device/board/hihope/rk3568/mcs/mcs_can/can_ctrl.h is the twin.
 *
 * Layout is a contract: any change bumps CAN_CTRL_VERSION and must be
 * mirrored on both sides.
 *
 * Location: per-port blocks in the last 4KB page of the 1MB MCS shared
 * window, one 0x100 page per handed-over controller:
 *   0x740ff000..0x740ff0ff  net control block (net_ctrl.h)
 *   0x740ff100..0x740ff1ff  CAN1 control block (can1_profile.h)
 *   0x740ff200..0x740ff2ff  CAN2 control block (can2_profile.h)
 *
 * The master publishes the platform facts ONCE at probe and never touches
 * CAN registers after the bus probe; the slave owns the whole CAN data
 * path and writes its own state here.
 */

#ifndef _CAN_CTRL_SLAVE_H_
#define _CAN_CTRL_SLAVE_H_

#include <stdint.h>

#define CAN_CTRL_SIZE           0x100

#define CAN_CTRL_MAGIC          0x43414E31u  /* "CAN1" (protocol id, not
                                              * a port number - the port
                                              * lives in port_index) */
#define CAN_CTRL_VERSION        1

/* master_state / slave_state */
#define CAN_CTRL_STATE_OFFLINE  0
#define CAN_CTRL_STATE_READY    1
#define CAN_CTRL_STATE_RUNNING  2
#define CAN_CTRL_STATE_FAULT    3

/* port_index */
#define CAN_PORT_CAN1           1
#define CAN_PORT_CAN2           2

struct can_ctrl {
    volatile uint32_t magic;        /* CAN_CTRL_MAGIC when master is up */
    volatile uint32_t version;      /* CAN_CTRL_VERSION */
    volatile uint32_t master_state; /* written by master (mcs_can.ko) */
    volatile uint32_t slave_state;  /* written by slave (Can runtime) */

    volatile uint32_t port_index;   /* CAN_PORT_CAN1 / CAN_PORT_CAN2 */
    volatile uint32_t base_phys;    /* 0xfe580000 / 0xfe590000 */
    volatile uint32_t intid;        /* 34 / 35 = GIC_SPI 2/3 + 32 (slave-owned) */
    volatile uint32_t baudclk_hz;   /* READ-BACK CLK_CANx rate (bit timing) */

    volatile uint32_t pclk_hz;      /* PCLK_CANx rate (informational) */
    volatile uint32_t cpu_version;  /* 3 = V2 silicon (no txtorx EXT
                                     * loopback), <3 = V1, 0 = unknown */
    volatile uint32_t _reserved[5]; /* pad to 64B (cache line) */
};

#endif /* _CAN_CTRL_SLAVE_H_ */
