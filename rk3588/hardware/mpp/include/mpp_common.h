/*
 * Copyright 2015 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MPP_COMMON_H__
#define __MPP_COMMON_H__

#include "rk_type.h"

#define MPP_TAG_SIZE            32

inline RK_S32 MPP_ALIGN(RK_S32 x, RK_S32 a)
{
    return (((x) + (a) - 1) & ~((a) - 1));
}
#include <stdio.h>
#if defined(_WIN32) && !defined(__MINGW32CE__)
#define fseeko                  _fseeki64

#include <direct.h>
#include <io.h>
#include <sys/stat.h>

#define chdir                   _chdir
#define mkdir                   _mkdir
#define access                  _access
#define off_t                   _off_t

#define R_OK                    4 /* Test for read permission. */
#define W_OK                    2 /* Test for write permission. */
#define X_OK                    1 /* Test for execute permission. */
#define F_OK                    0 /* Test for existence. */

#elif defined(__MINGW32CE__)
#define fseeko                  fseeko64
#else
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
inline int mkdir(const char *x)
{
    return mkdir(x, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define RETURN                Return
#define FAILED                failed

#define ARG_T(t)                t
#define ARG_N(a,b,c,d,N,...)    N
#define ARG_N_HELPER(...)       ARG_T(ARG_N(__VA_ARGS__))
#define COUNT_ARG(...)          ARG_N_HELPER(__VA_ARGS__,4,3,2,1,0)

#define SZ_1K                   (1024)
#define SZ_2K                   (SZ_1K*2)
#define SZ_4K                   (SZ_1K*4)
#define SZ_8K                   (SZ_1K*8)
#define SZ_16K                  (SZ_1K*16)
#define SZ_32K                  (SZ_1K*32)
#define SZ_64K                  (SZ_1K*64)
#define SZ_128K                 (SZ_1K*128)
#define SZ_256K                 (SZ_1K*256)
#define SZ_512K                 (SZ_1K*512)
#define SZ_1M                   (SZ_1K*SZ_1K)
#define SZ_2M                   (SZ_1M*2)
#define SZ_4M                   (SZ_1M*4)
#define SZ_8M                   (SZ_1M*8)
#define SZ_16M                  (SZ_1M*16)
#define SZ_32M                  (SZ_1M*32)
#define SZ_64M                  (SZ_1M*64)
#define SZ_80M                  (SZ_1M*80)
#define SZ_128M                 (SZ_1M*128)

#define TSVC_MAX_TID_LEVEL_1    0
#define TSVC_MAX_TID_LEVEL_2    1
#define TSVC_MAX_TID_LEVEL_3    2
#define TSVC_MAX_TID_LEVEL_4    3

#define MJPEG_DEFAULT_QP        10
#define H264_MIN_QP             10
#define H264_MIN_QP_I           10
#define H264_MAX_QP             51
#define H264_MAX_QP_I           51
#define H264_QP_STEP            4
#define H264_QP_DELTA_IP        3
#define VP8_DEFAULT_QP          56
#define H264_DEFAULT_QP         26
#define VP8_MAX_QP              127

#define TEMPORAL_ID_LAYER_0     0
#define TEMPORAL_ID_LAYER_1     1
#define TEMPORAL_ID_LAYER_2     2
#define TEMPORAL_ID_LAYER_3     3

#define NO_REPETITION               0
#define DEFAULT_REF_VALUE       0
#define REF_ARG_VALUE_1         1

#define STRIDE_MULTIPLIER_RGB565    2
#define STRIDE_MULTIPLIER_BGR888    3
#define STRIDE_MULTIPLIER_RGBA8888  4
#define RGB565_PIXEL_SIZE_RATIO 3
#define RGB101010_PIXEL_SIZE_RATIO  4
#define YUV420_PIXEL_SIZE_RATIO_NUMERATOR 3
#define YUV420_PIXEL_SIZE_RATIO_DENOMINATOR 2
#define YUV420_PIXEL_RATIO_DENOMINATOR  2

#define VPU_FLAG_DEFAULT    0
#define IS_NON_REF_FALSE    0
#define IS_NON_REF_TRUE     1

#define BPP_RGB_16BIT_FORMATS 2  // For RGB444/555/565 formats
#define BPP_DEFAULT_FORMATS   4  // For other formats

typedef enum {
    TSVC0_ST_0_INDEX = 0,
    TSVC0_ST_COUNT = 1
} Tsvc0ConfigIndex;

/* TSVC2 配置索引 */
typedef enum {
    TSVC2_ST_0_INDEX = 0,
    TSVC2_ST_1_INDEX,
    TSVC2_ST_2_INDEX,
    TSVC2_ST_COUNT = 3
} Tsvc2ConfigIndex;

/* TSVC3 配置索引 */
typedef enum {
    TSVC3_ST_0_INDEX = 0,
    TSVC3_ST_1_INDEX,
    TSVC3_ST_2_INDEX,
    TSVC3_ST_3_INDEX,
    TSVC3_ST_4_INDEX,
    TSVC3_ST_COUNT = 5
} Tsvc3ConfigIndex;

/* TSVC4 配置索引 */
typedef enum {
    TSVC4_ST_0_INDEX = 0,
    TSVC4_ST_1_INDEX,
    TSVC4_ST_2_INDEX,
    TSVC4_ST_3_INDEX,
    TSVC4_ST_4_INDEX,
    TSVC4_ST_5_INDEX,
    TSVC4_ST_6_INDEX,
    TSVC4_ST_7_INDEX,
    TSVC4_ST_8_INDEX,
    TSVC4_ST_COUNT = 9
} Tsvc4ConfigIndex;

#define MPP_REQ_WRITE_INDEX     0
#define MPP_REQ_READ_INDEX      1
#define MPP_REQ_PATCH_INDEX     2
#define MPP_BASIC_REQ_COUNT     2
#define MPP_FULL_REQ_COUNT      3

#ifdef __cplusplus
extern "C" {
#endif

RK_S32 mpp_log2(RK_U32 v);
RK_S32 mpp_log2_16bit(RK_U32 v);

static inline RK_S32 mpp_ceil_log2(RK_S32 x)
{
    return mpp_log2((x - 1) << 1);
}

static inline RK_S32 mpp_clip(RK_S32 a, RK_S32 amin, RK_S32 amax)
{
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

static inline RK_U32 mpp_is_32bit()
{
    const int IS_32BIT = 1;
    const int IS_NOT_32BIT = 0;
    const size_t SIZE_OF_UINT32 = sizeof(uint32_t);
    return ((SIZE_OF_UINT32 == sizeof(void *)) ? (IS_32BIT) : (IS_NOT_32BIT));
}

static inline RK_S32 mpp_dup(RK_S32 fd)
{
    /* avoid stdin / stdout / stderr so start from 3 */
    const int MINIMUM_SAFE_FD = 3;
#ifdef F_DUPFD_CLOEXEC
    return fcntl(fd, F_DUPFD_CLOEXEC, MINIMUM_SAFE_FD);
#else
    RK_S32 new_fd = -1;

    new_fd = fcntl(fd, F_DUPFD, MINIMUM_SAFE_FD);
    if (new_fd == -1)
        return -1;

    if (fcntl(new_fd, F_SETFD, FD_CLOEXEC) == -1)
        return -1;

    return new_fd;
#endif
}

RK_S32 axb_div_c(RK_S32 a, RK_S32 b, RK_S32 c);
RK_U32 mpp_align_16(RK_U32 val);
RK_U32 mpp_align_64(RK_U32 val);
RK_U32 mpp_align_128(RK_U32 val);
RK_U32 mpp_align_256_odd(RK_U32 val);
RK_U32 mpp_align_128_odd_plus_64(RK_U32 val);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_COMMON_H__*/

