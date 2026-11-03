/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (c) 2026 Rockchip Electronics Co., Ltd.
 *
 * mpp_test.h - Unified assertion and check helpers for mpp unit tests
 *
 * 1. Test assertions: mpp_test_assert_* record a failure into a caller-owned
 *    rk_s32 result variable (declared via MPP_RET_VARS) and let the test
 *    continue, so a single run reports every failure instead of stopping
 *    at the first one.
 *
 * 2. Test infrastructure: MppTestCase table + mpp_test_run helpers.
 *
 * 3. Test option parsing (mpp-opt-style, header-only).
 *
 * Usage:
 *   static rk_s32 my_test(void *ctx)
 *   {
 *       MPP_RET_VARS;
 *
 *       mpp_test_assert_eq("a == b", a, b, &_mpp_ret);
 *       return _mpp_ret;
 *   }
 */

#ifndef MPP_TEST_H
#define MPP_TEST_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "mpp_log.h"
#include "mpp_common.h"

/* Declares the result variable conventionally named _mpp_ret that the
 * mpp_test_assert_* helpers write to: rk_ok passes, rk_nok fails. */
#define MPP_RET_VARS rk_s32 _mpp_ret = rk_ok

/* ===========================================================================
 * Section 1: Test assertions
 * Convention: *ret < 0 means failure, rk_ok means success.
 * =========================================================================== */

static inline void mpp_test_fail(const char *msg, const char *func,
                                 rk_s32 line, rk_s32 *ret)
{
    mpp_loge("  FAIL: %s at %s:%d\n", msg, func, line);
    *ret = rk_nok;
}

static inline void mpp_test_vformat(char *buf, rk_u32 size, const char *fmt,
                                    va_list ap)
{
    if (vsnprintf(buf, size, fmt, ap) < 0)
        buf[0] = '\0';
}

static inline void mpp_test_failf(rk_s32 *ret, const char *func, rk_s32 line,
                                  const char *fmt, ...)
{
    char buf[128];
    va_list ap;

    va_start(ap, fmt);
    mpp_test_vformat(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    mpp_test_fail(buf, func, line, ret);
}

static inline void mpp_test_fail_null(const char *msg, const void *p,
                                      const char *func, rk_s32 line,
                                      rk_s32 *ret)
{
    mpp_loge("  FAIL: %s (expected NULL, got %p) at %s:%d\n", msg, p,
             func, line);
    *ret = rk_nok;
}

static inline void mpp_test_fail_not_null(const char *msg, const char *func,
                                          rk_s32 line, rk_s32 *ret)
{
    mpp_loge("  FAIL: %s (unexpected NULL) at %s:%d\n", msg, func, line);
    *ret = rk_nok;
}

static inline void mpp_test_assert_true(const char *msg, rk_s32 pass,
                                        rk_s32 *ret)
{
    if (!pass)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_false(const char *msg, rk_s32 pass,
                                         rk_s32 *ret)
{
    if (pass)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_eq(const char *msg, rk_s64 exp, rk_s64 got,
                                      rk_s32 *ret)
{
    if (exp != got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_neq(const char *msg, rk_s64 exp,
                                       rk_s64 got, rk_s32 *ret)
{
    if (exp == got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_gt(const char *msg, rk_s64 exp, rk_s64 got,
                                      rk_s32 *ret)
{
    if (exp <= got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_gte(const char *msg, rk_s64 exp,
                                       rk_s64 got, rk_s32 *ret)
{
    if (exp < got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_lt(const char *msg, rk_s64 exp, rk_s64 got,
                                      rk_s32 *ret)
{
    if (exp >= got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_lte(const char *msg, rk_s64 exp,
                                       rk_s64 got, rk_s32 *ret)
{
    if (exp > got)
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_null(const char *msg, const void *p,
                                        rk_s32 *ret)
{
    if (p != NULL)
        mpp_test_fail_null(msg, p, __func__, __LINE__, ret);
}

static inline void mpp_test_assert_not_null(const char *msg, const void *p,
                                            rk_s32 *ret)
{
    if (p == NULL)
        mpp_test_fail_not_null(msg, __func__, __LINE__, ret);
}

static inline rk_s32 mpp_test_mem_eq(const void *exp, const void *got,
                                     rk_u32 size)
{
    if (!exp || !got)
        return 0;

    return memcmp(exp, got, size) == 0;
}

static inline void mpp_test_assert_mem_eq(const char *msg, const void *exp,
                                          const void *got, rk_u32 size,
                                          rk_s32 *ret)
{
    if (!mpp_test_mem_eq(exp, got, size))
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

static inline rk_s32 mpp_test_str_eq(const char *exp, const char *got)
{
    if (!exp || !got)
        return 0;

    return strcmp(exp, got) == 0;
}

static inline void mpp_test_assert_str_eq(const char *msg, const char *exp,
                                          const char *got, rk_s32 *ret)
{
    if (!mpp_test_str_eq(exp, got))
        mpp_test_fail(msg, __func__, __LINE__, ret);
}

/* ===========================================================================
 * Section 2: Test infrastructure
 * =========================================================================== */

typedef struct MppTestCase_t {
    const char  *name;
    rk_u32      flag;
    rk_u32      level;
    rk_s32      (*func)(void *ctx);
} MppTestCase;

static inline void mpp_test_report(const char *name, rk_s32 ok)
{
    mpp_logi("test %s %s\n", name, ok ? "success" : "failed");
}

static inline void mpp_test_run_one(void *ctx, MppTestCase *cases, rk_s32 idx,
                                    rk_s32 *ret)
{
    rk_s32 r = cases[idx].func(ctx);

    mpp_test_report(cases[idx].name, r >= 0);
    if (r < 0)
        *ret = rk_nok;
}

static inline void mpp_test_run(void *ctx, MppTestCase *cases, rk_u32 n,
                                rk_s32 *ret)
{
    rk_u32 i;

    for (i = 0; i < n; i++)
        mpp_test_run_one(ctx, cases, (rk_s32)i, ret);
}

static inline void mpp_test_run_range(void *ctx, MppTestCase *cases, rk_s32 n,
                                      rk_s32 start, rk_s32 end, rk_s32 *ret)
{
    rk_s32 i;

    if (n <= 0)
        return;

    start = mpp_max(0, mpp_min(start, n - 1));
    end = mpp_max(start, mpp_min(end, n - 1));
    for (i = start; i <= end; i++)
        mpp_test_run_one(ctx, cases, i, ret);
}

static inline void mpp_test_start(const char *name)
{
    (void)name;
    mpp_logi("start\n");
}

static inline void mpp_test_end(const char *name, rk_s32 ret)
{
    (void)name;
    mpp_logi("done %s\n", ret ? "failed" : "success");
}

/* ===========================================================================
 * Section 3: Test option parsing (mpp-opt-style, header-only)
 * =========================================================================== */

#define MPP_FLAG_VERBOSE (1 << 0)

static inline void mpp_test_verbose(rk_u32 flag, const char *fmt, ...)
{
    char buf[128];
    va_list ap;

    if (!(flag & MPP_FLAG_VERBOSE))
        return;

    va_start(ap, fmt);
    mpp_test_vformat(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    mpp_logi("%s", buf);
}

typedef rk_s32 (*MppTestOptFn)(void *ctx, const char *next);

typedef struct MppTestOpt_t {
    char          opt;       /* short option: 'v', 'd', 'h' */
    const char   *long_opt;  /* long option: "verbose", "debug" */
    const char   *help;      /* help text */
    MppTestOptFn  fn;        /* handler, returns argc consumed (1=flag, 2=flag+value) */
} MppTestOpt;

/* Length of the "--" prefix used for long options, and the total
 * length of a short option ("-x"), used to skip the prefix when matching. */
#define MPP_LONG_OPT_PREFIX_LEN 2
#define MPP_SHORT_OPT_LEN       2

static inline rk_s32 mpp_test_match_long(const char *arg, MppTestOpt *opt)
{
    return opt->long_opt && arg[1] == '-' &&
           !strcmp(arg + MPP_LONG_OPT_PREFIX_LEN, opt->long_opt);
}

static inline rk_s32 mpp_test_match_short(const char *arg, MppTestOpt *opt)
{
    return opt->opt && arg[1] == opt->opt && arg[MPP_SHORT_OPT_LEN] == '\0';
}

static inline rk_s32 mpp_test_match_opt(const char *arg, MppTestOpt *opts,
                                        rk_s32 nopts)
{
    rk_s32 j;

    for (j = 0; j < nopts; j++) {
        if (mpp_test_match_long(arg, &opts[j]) ||
            mpp_test_match_short(arg, &opts[j]))
            return j;
    }

    return -1;
}

static inline rk_s32 mpp_test_next_opt(MppTestOpt *opts, rk_s32 nopts,
                                       rk_s32 argc, char **argv, rk_s32 i)
{
    if (i >= argc || argv[i][0] != '-')
        return -1;

    return mpp_test_match_opt(argv[i], opts, nopts);
}

static inline rk_s32 mpp_test_apply_opt(void *ctx, MppTestOpt *opt,
                                        rk_s32 argc, char **argv, rk_s32 i)
{
    const char *next = (i + 1 < argc) ? argv[i + 1] : NULL;
    rk_s32 step = opt->fn ? mpp_max(1, opt->fn(ctx, next)) : 1;

    return i + step;
}

static inline rk_s32 mpp_test_parse_opts(void *ctx, MppTestOpt *opts,
                                         rk_s32 nopts, rk_s32 argc,
                                         char **argv)
{
    rk_s32 i = 1;
    rk_s32 j;

    while ((j = mpp_test_next_opt(opts, nopts, argc, argv, i)) >= 0)
        i = mpp_test_apply_opt(ctx, &opts[j], argc, argv, i);

    return i;
}

static inline void mpp_test_opt_help_one(MppTestOpt *opt)
{
    if (opt->long_opt)
        mpp_logi("  -%c, --%-12s %s\n", opt->opt, opt->long_opt, opt->help);
    else
        mpp_logi("  -%-17c %s\n", opt->opt, opt->help);
}

static inline void mpp_test_opt_help(const char *name, MppTestOpt *opts,
                                     rk_s32 nopts)
{
    rk_s32 i;

    mpp_logi("Usage: %s [options] [file] [start] [end]\n", name);
    mpp_logi("Options:\n");
    for (i = 0; i < nopts; i++)
        mpp_test_opt_help_one(&opts[i]);
}

#endif /* MPP_TEST_H */
