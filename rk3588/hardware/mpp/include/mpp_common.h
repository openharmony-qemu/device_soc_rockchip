/* SPDX-License-Identifier: Apache-2.0 OR MIT */
/*
 * Copyright (c) 2015 Rockchip Electronics Co., Ltd.
 */

#ifndef MPP_COMMON_H
#define MPP_COMMON_H

#include "rk_type.h"
#include "mpp_err.h"

#define MPP_TAG_SIZE            32

/*
 * G.PRE.02: the arithmetic helper macros (MPP_ABS/MPP_MIN/MPP_MAX and
 * derivatives) were converted to static inline functions below; all call
 * sites are runtime contexts, so no constant-expression usage is lost.
 * MPP_SWAP must remain a macro: it is a statement macro with a type
 * parameter, which an inline function cannot express.
 *
 * G.PRE.02: MPP_ARRAY_ELEMS_U/S, MPP_ALIGN_DOWN, MPP_ALIGN_GEN, MPP_ROUNDUP,
 * MPP_DIVUP and MPP_GENMASK were converted to static inline functions too.
 * MPP_ARRAY_ELEMS keeps its macro form: sizeof() needs the array type, which
 * an inline function cannot recover from a decayed pointer argument.
 * MPP_ALIGN keeps its macro form for the same reason as MPP_SWAP (generic
 * integer width) and because 90 call sites rely on it in init expressions.
 */
#define MPP_SWAP(type, a, b)    do {type SWAP_tmp = b; b = a; a = SWAP_tmp;} while(0)
#define MPP_ARRAY_ELEMS(a)      (sizeof(a) / sizeof((a)[0]))
/* Thin forwarders keep the MPP_ARRAY_ELEMS_U(arr) call form; the
 * computation itself now lives in the inline functions below. */
#define MPP_ARRAY_ELEMS_U(a)    mpp_array_elems_u(sizeof(a), sizeof((a)[0]))
#define MPP_ARRAY_ELEMS_S(a)    mpp_array_elems_s(sizeof(a), sizeof((a)[0]))
#define MPP_ALIGN(x, a)         (((x)+(a)-1)&~((a)-1))
#define MPP_VSWAP(a, b)         { a ^= b; b ^= a; a ^= b; }

#define MPP_RB16(x)  ((((const RK_U8*)(x))[0] << 8) | ((const RK_U8*)(x))[1])
#define MPP_WB16(p, d) do { \
        ((RK_U8*)(p))[1] = (d); \
        ((RK_U8*)(p))[0] = (d)>>8; } while(0)

#define MPP_RL16(x)  ((((const RK_U8*)(x))[1] << 8) | \
                     ((const RK_U8*)(x))[0])
#define MPP_WL16(p, d) do { \
        ((RK_U8*)(p))[0] = (d); \
        ((RK_U8*)(p))[1] = (d)>>8; } while(0)

#define MPP_RB32(x)  ((((const RK_U8*)(x))[0] << 24) | \
                     (((const RK_U8*)(x))[1] << 16) | \
                     (((const RK_U8*)(x))[2] <<  8) | \
                     ((const RK_U8*)(x))[3])
#define MPP_WB32(p, d) do { \
        ((RK_U8*)(p))[3] = (d); \
        ((RK_U8*)(p))[2] = (d)>>8; \
        ((RK_U8*)(p))[1] = (d)>>16; \
        ((RK_U8*)(p))[0] = (d)>>24; } while(0)

#define MPP_RL32(x) ((((const RK_U8*)(x))[3] << 24) | \
                    (((const RK_U8*)(x))[2] << 16) | \
                    (((const RK_U8*)(x))[1] <<  8) | \
                    ((const RK_U8*)(x))[0])
#define MPP_WL32(p, d) do { \
        ((RK_U8*)(p))[0] = (d); \
        ((RK_U8*)(p))[1] = (d)>>8; \
        ((RK_U8*)(p))[2] = (d)>>16; \
        ((RK_U8*)(p))[3] = (d)>>24; } while(0)

#define MPP_RB64(x)  (((RK_U64)((const RK_U8*)(x))[0] << 56) | \
                     ((RK_U64)((const RK_U8*)(x))[1] << 48) | \
                     ((RK_U64)((const RK_U8*)(x))[2] << 40) | \
                     ((RK_U64)((const RK_U8*)(x))[3] << 32) | \
                     ((RK_U64)((const RK_U8*)(x))[4] << 24) | \
                     ((RK_U64)((const RK_U8*)(x))[5] << 16) | \
                     ((RK_U64)((const RK_U8*)(x))[6] <<  8) | \
                     (RK_U64)((const RK_U8*)(x))[7])
#define MPP_WB64(p, d) do { \
        ((RK_U8*)(p))[7] = (d);     \
        ((RK_U8*)(p))[6] = (d)>>8;  \
        ((RK_U8*)(p))[5] = (d)>>16; \
        ((RK_U8*)(p))[4] = (d)>>24; \
        ((RK_U8*)(p))[3] = (d)>>32; \
        ((RK_U8*)(p))[2] = (d)>>40; \
        ((RK_U8*)(p))[1] = (d)>>48; \
        ((RK_U8*)(p))[0] = (d)>>56; } while(0)

#define MPP_RL64(x)  (((RK_U64)((const RK_U8*)(x))[7] << 56) | \
                     ((RK_U64)((const RK_U8*)(x))[6] << 48) | \
                     ((RK_U64)((const RK_U8*)(x))[5] << 40) | \
                     ((RK_U64)((const RK_U8*)(x))[4] << 32) | \
                     ((RK_U64)((const RK_U8*)(x))[3] << 24) | \
                     ((RK_U64)((const RK_U8*)(x))[2] << 16) | \
                     ((RK_U64)((const RK_U8*)(x))[1] <<  8) | \
                     (RK_U64)((const RK_U8*)(x))[0])
#define MPP_WL64(p, d) do { \
        ((RK_U8*)(p))[0] = (d);     \
        ((RK_U8*)(p))[1] = (d)>>8;  \
        ((RK_U8*)(p))[2] = (d)>>16; \
        ((RK_U8*)(p))[3] = (d)>>24; \
        ((RK_U8*)(p))[4] = (d)>>32; \
        ((RK_U8*)(p))[5] = (d)>>40; \
        ((RK_U8*)(p))[6] = (d)>>48; \
        ((RK_U8*)(p))[7] = (d)>>56; } while(0)

#define MPP_RB24(x)  ((((const RK_U8*)(x))[0] << 16) | \
                     (((const RK_U8*)(x))[1] <<  8) | \
                     ((const RK_U8*)(x))[2])
#define MPP_WB24(p, d) do { \
        ((RK_U8*)(p))[2] = (d); \
        ((RK_U8*)(p))[1] = (d)>>8; \
        ((RK_U8*)(p))[0] = (d)>>16; } while(0)

#define MPP_RL24(x)  ((((const RK_U8*)(x))[2] << 16) | \
                     (((const RK_U8*)(x))[1] <<  8) | \
                     ((const RK_U8*)(x))[0])

#define MPP_WL24(p, d) do { \
        ((RK_U8*)(p))[0] = (d); \
        ((RK_U8*)(p))[1] = (d)>>8; \
        ((RK_U8*)(p))[2] = (d)>>16; } while(0)

#include <stdio.h>
#if defined(__MINGW32CE__)
#define fseeko                  fseeko64
#else
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define mkdir(x)                mkdir(x, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#endif

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define __RETURN                __Return
#define __FAILED                __failed

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

#define BPP_RGB_16BIT_FORMATS 2  /* For RGB444/555/565 formats */
#define BPP_DEFAULT_FORMATS   4  /* For other formats */
#ifdef __cplusplus
extern "C" {
#endif

RK_S32 mpp_log2(RK_U32 v);
RK_S32 mpp_log2_16bit(RK_U32 v);

static __inline RK_S32 mpp_ceil_log2(RK_S32 x)
{
    return mpp_log2(((RK_U32)x - 1U) << 1);
}

static __inline RK_S32 mpp_clip(RK_S32 a, RK_S32 amin, RK_S32 amax)
{
    if      (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

static inline rk_u32 mpp_array_elems_u(const size_t bytes, const size_t elem_size)
{
    if (elem_size == 0)
        return 0;

    return (rk_u32)(bytes / elem_size);
}

static inline rk_s32 mpp_array_elems_s(const size_t bytes, const size_t elem_size)
{
    if (elem_size == 0)
        return 0;

    return (rk_s32)(bytes / elem_size);
}

static inline RK_U32 mpp_align_down(RK_U32 x, RK_U32 a)
{
    if (a == 0)
        return x;

    return x & ~(a - 1);
}

static inline RK_U32 mpp_align_gen(RK_U32 x, RK_U32 a)
{
    if (a == 0)
        return x;

    return (x + a - 1) / a * a;
}

static inline RK_U32 mpp_roundup(RK_U32 n, RK_U32 m)
{
    if (n == 0)
        return 0;

    return mpp_align_gen(m, n);
}

static inline RK_U32 mpp_divup(RK_U32 n, RK_U32 m)
{
    if (n == 0)
        return 0;

    return mpp_roundup(n, m) / n;
}

static inline RK_U64 mpp_genmask(RK_U32 h, RK_U32 l)
{
    return ((1ULL << (h + 1)) - 1) & ~((1ULL << l) - 1);
}

static inline RK_S32 mpp_abs(RK_S32 x)
{
    return (x < 0) ? -x : x;
}

static inline RK_S32 mpp_min(RK_S32 a, RK_S32 b)
{
    return (a > b) ? b : a;
}

static inline RK_S32 mpp_min3(RK_S32 a, RK_S32 b, RK_S32 c)
{
    return mpp_min(mpp_min(a, b), c);
}

static inline RK_S32 mpp_min4(RK_S32 a, RK_S32 b, RK_S32 c, RK_S32 d)
{
    return mpp_min(a, mpp_min3(b, c, d));
}

static inline RK_S32 mpp_max(RK_S32 a, RK_S32 b)
{
    return (a > b) ? a : b;
}

static inline RK_S32 mpp_max3(RK_S32 a, RK_S32 b, RK_S32 c)
{
    return mpp_max(mpp_max(a, b), c);
}

static inline RK_S32 mpp_max4(RK_S32 a, RK_S32 b, RK_S32 c, RK_S32 d)
{
    return mpp_max(a, mpp_max3(b, c, d));
}

static inline RK_S32 mpp_div(RK_S32 a, RK_S32 b)
{
    return (b != 0) ? a / b : a;
}

static inline RK_S32 mpp_clip3(RK_S32 l, RK_S32 h, RK_S32 v)
{
    return (v < l) ? l : ((v > h) ? h : v);
}

static inline RK_S32 mpp_sign(RK_S32 a)
{
    return (a < 0) ? -1 : 1;
}

static inline RK_S32 mpp_div_sign(RK_S32 a, RK_S32 b)
{
    return (a + mpp_sign(a) * b / 2) / b;
}

/* minimum fd for duplication: skip stdin / stdout / stderr */
#define MPP_FD_MIN              3

static __inline RK_S32 mpp_dup(RK_S32 fd)
{
#ifdef F_DUPFD_CLOEXEC
    return fcntl(fd, F_DUPFD_CLOEXEC, MPP_FD_MIN);
#else
    RK_S32 new_fd = -1;

    new_fd = fcntl(fd, F_DUPFD, MPP_FD_MIN);
    if (new_fd == -1)
        return -1;

    if (fcntl(new_fd, F_SETFD, FD_CLOEXEC) == -1)
        return -1;

    return new_fd;
#endif
}

RK_S32 axb_div_c(RK_S32 a, RK_S32 b, RK_S32 c);
RK_U32 mpp_align_8(RK_U32 val);
RK_U32 mpp_align_16(RK_U32 val);
RK_U32 mpp_align_64(RK_U32 val);
RK_U32 mpp_align_128(RK_U32 val);
RK_U32 mpp_align_256_odd(RK_U32 val);
RK_U32 mpp_align_128_odd_plus_64(RK_U32 val);
RK_U32 mpp_align_wxh2yuv420(RK_U32 val);
RK_U32 mpp_align_wxh2yuv422(RK_U32 val);
RK_U32 mpp_align_wxh2yuv444(RK_U32 val);

/* Clip a signed integer to the range [0, 2^n - 1] */
RK_U32 mpp_clip_uint_pow2(RK_S32 val, RK_S32 n);
/* Divide an unsigned integer x by 2^n and round to nearest integer */
RK_U64 mpp_round_pow2(RK_U64 x, RK_U16 n);
/* Divide a signed integer x by 2^n and round to nearest integer */
RK_S64 mpp_round_pow2_signed(RK_S64 x, RK_U16 n);

MPP_RET mpp_mkdir_p(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* MPP_COMMON_H */

