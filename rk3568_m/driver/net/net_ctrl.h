/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * net_ctrl.h - shared control block between the Linux master (mcs_net.ko)
 * and the LiteOS-M slave (net_dwmac.c) for the RK3568 GMAC1 real-time port.
 *
 * Layout is a contract: any change bumps NET_CTRL_VERSION and must be
 * mirrored into the master-side copy (device/board/hihope/rk3568/mcs/
 * mcs_net/net_ctrl.h).
 *
 * Location: last 4KB page of the 1MB MCS shared window
 *   0x74000000..0x740fefff  openamp vring / rpmsg buffers (owned by mcs_km)
 *   0x740ff000..0x740fffff  this control block
 */

#ifndef _NET_CTRL_H_
#define _NET_CTRL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NET_CTRL_PHYS        0x740FF000UL
#define NET_CTRL_SIZE        0x1000

#define NET_CTRL_MAGIC       0x45434E31UL  /* "ECN1" */
#define NET_CTRL_VERSION     1

/* master_state / slave_state */
#define NET_STATE_OFFLINE    0
#define NET_STATE_READY      1
#define NET_STATE_RUNNING    2
#define NET_STATE_FAULT      3

struct net_ctrl {
    uint32_t magic;         /* NET_CTRL_MAGIC when master is up */
    uint32_t version;       /* NET_CTRL_VERSION */
    uint32_t master_state;  /* written by master (mcs_net.ko) */
    uint32_t phy_id;        /* PHYSID1<<16 | PHYSID2 */

    /* link snapshot, refreshed by the master 1s delayed work */
    uint32_t link_up;       /* 0/1 */
    uint32_t speed_mbps;    /* 100 */
    uint32_t full_duplex;   /* 0/1 */
    uint32_t valid;         /* snapshot has been sampled at least once */
    uint64_t sample_time_ns;/* master ktime_get_boottime_ns() */
    uint32_t link_changes;  /* count of link transitions observed */

    uint32_t slave_state;   /* written by slave (Net runtime state) */

    /* keep the struct a multiple of 64B (cache line) for future fields */
    uint8_t  _reserved[64 - 52];
};

#ifdef __cplusplus
}
#endif

#endif /* _NET_CTRL_H_ */
