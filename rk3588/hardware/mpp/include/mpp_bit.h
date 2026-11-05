/* SPDX-License-Identifier: Apache-2.0 OR MIT */
/*
 * Copyright (c) 2025 Rockchip Electronics Co., Ltd.
 */

#ifndef MPP_BIT_H
#define MPP_BIT_H

#define MPP_BIT(n) (1UL << (n))

/*
 * The macros in this header must remain macros: they are used in enum
 * initializers and other integer-constant-expression contexts
 * (rk_mpi_cmd.h command ids, mpp_allocator.h flag masks), where a
 * function call is not permitted. MPP_FLAG_OR dispatches to the
 * MPP_FLAG_NOR variants by argument count; the variants are its
 * implementation, not standalone APIs.
 */
#define MPP_FLAG_1OR(a) ((RK_U32)(a))
#define MPP_FLAG_2OR(a, b) ((RK_U32)(a) | (RK_U32)(b))
#define MPP_FLAG_3OR(a, b, c) ((RK_U32)(a) | (RK_U32)(b) | (RK_U32)(c))
#define MPP_FLAG_4OR(a, b, c, d) ((RK_U32)(a) | (RK_U32)(b) | (RK_U32)(c) | (RK_U32)(d))
#define MPP_FLAG_5OR(a, b, c, d, e) ((RK_U32)(a) | (RK_U32)(b) | (RK_U32)(c) | (RK_U32)(d) | (RK_U32)(e))
#define MPP_FLAG_6OR(a, b, c, d, e, f)                                                                                 \
    ((RK_U32)(a) | (RK_U32)(b) | (RK_U32)(c) | (RK_U32)(d) | (RK_U32)(e) | (RK_U32)(f))

#define MPP_FLAG_OR_HELPER(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define MPP_FLAG_OR(...)                                                                                               \
    MPP_FLAG_OR_HELPER(__VA_ARGS__, MPP_FLAG_6OR, MPP_FLAG_5OR, MPP_FLAG_4OR, MPP_FLAG_3OR, MPP_FLAG_2OR,              \
                       MPP_FLAG_1OR)                                                                                   \
    (__VA_ARGS__)

#endif /* MPP_BIT_H */
