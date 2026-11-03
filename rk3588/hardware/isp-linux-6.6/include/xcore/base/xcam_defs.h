/*
 * xcam_defs.h - macros defines
 *
 *  Copyright (c) 2014-2015 Intel Corporation
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
 *
 * Author: Wind Yuan <feng.yuan@intel.com>
 */

#ifndef XCAM_DEFS_H
#define XCAM_DEFS_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(__linux__)
#include <sys/time.h>
#include <time.h>
#include "unistd.h"
#endif

/*
 * Function-like macros are avoided below where the same semantics can be
 * expressed with an inline function. The remaining macros are either
 * preprocessor-level constructs (token pasting, variadic argument lists,
 * declaration specifiers) or type-generic helpers that cannot be turned into
 * a single C function without losing type transparency.
 */
#define XCAM_ASSERT(exp)  assert(exp)

#ifdef  __cplusplus
#define XCAM_BEGIN_DECLARE  extern "C" {
#define XCAM_END_DECLARE    }
#else
#define XCAM_BEGIN_DECLARE
#define XCAM_END_DECLARE
#endif

#ifndef XCAM_USER
#define XCAM_USER
#endif

static inline void xcam_unused(const void *variable)
{
    (void)variable;
}

#define XCAM_CONSTRUCTOR(obj, TYPE, ...) new (&obj) TYPE(## __VA_ARGS__)
#define XCAM_DESTRUCTOR(obj, TYPE) (obj).~TYPE()

/*
 * XCAM_MAX / XCAM_MIN / XCAM_CLAMP stay function-like macros on purpose:
 * they are type-generic (int, size_t, RkAiq...). A plain C function cannot
 * preserve that, and _Generic is not portable to every compiler used here.
 */
#define XCAM_MAX(a, b)  ((a) > (b) ? (a) : (b))
#define XCAM_MIN(a, b)  ((a) < (b) ? (a) : (b))
#define XCAM_CLAMP(v, min, max)   \
    (((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)))

#define XCAM_FAIL_RETURN(LEVEL, exp, ret, msg, ...)         \
    if (!(exp)) {                                           \
        XCAM_LOG_##LEVEL (msg, ## __VA_ARGS__);             \
        return (ret);                                       \
    }

#define XCAM_DEAD_COPY(ClassObj)                \
        ClassObj (const ClassObj&);             \
        ClassObj & operator= (const ClassObj&)  \


static inline const char *xcam_str(const char *str)
{
    return str ? str : "null";
}

static inline const char *XCAM_STR(const char *str)
{
    return xcam_str(str);
}

static inline const char *xcam_bool2str(int value)
{
    return value ? "true" : "false";
}

static inline const char *XCAM_BOOL2STR(bool value)
{
    return xcam_bool2str(value ? 1 : 0);
}

static inline int xcam_double_equal(double a, double b, double tolerance)
{
    return (a >= (b - tolerance)) && (a <= (b + tolerance));
}

static inline int XCAM_DOUBLE_EQUAL(double a, double b, double tolerance)
{
    return xcam_double_equal(a, b, tolerance);
}

static inline int xcam_double_equal_around(double a, double b)
{
    const double tolerance = 0.000001;
    return xcam_double_equal(a, b, tolerance);
}

static inline int XCAM_DOUBLE_EQUAL_AROUND(double a, double b)
{
    return xcam_double_equal_around(a, b);
}

#define XCAM_GAMMA_TABLE_SIZE 256
#define XCAM_MAX_STR_SIZE 4096
#undef XCAM_CL_MAX_STR_SIZE
#define XCAM_CL_MAX_STR_SIZE 1024

#define XCAM_USEC_PER_SEC 1000000LL
#define XCAM_NSEC_PER_USEC 1000

static inline int64_t xcam_timespec_2_nsec(struct timespec ts)
{
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline int64_t xcam_timespec_2_usec(struct timespec ts)
{
    return (int64_t)ts.tv_sec * XCAM_USEC_PER_SEC + ts.tv_nsec / XCAM_NSEC_PER_USEC;
}

static inline int64_t xcam_timeval_2_usec(struct timeval tv)
{
    return (int64_t)tv.tv_sec * XCAM_USEC_PER_SEC + tv.tv_usec;
}

static inline int64_t XCAM_TIMESPEC_2_NSEC(struct timespec ts)
{
    return xcam_timespec_2_nsec(ts);
}

static inline int64_t XCAM_TIMESPEC_2_USEC(struct timespec ts)
{
    return xcam_timespec_2_usec(ts);
}

static inline int64_t XCAM_TIMEVAL_2_USEC(struct timeval tv)
{
    return xcam_timeval_2_usec(tv);
}

static inline int64_t xcam_timestamp_2_seconds(int64_t t)
{
    return t / XCAM_USEC_PER_SEC;
}

static inline int64_t xcam_seconds_2_timestamp(int64_t sec)
{
    return sec * XCAM_USEC_PER_SEC;
}

static inline int64_t XCAM_TIMESTAMP_2_SECONDS(int64_t t)
{
    return xcam_timestamp_2_seconds(t);
}

static inline int64_t XCAM_SECONDS_2_TIMESTAMP(int64_t t)
{
    return xcam_seconds_2_timestamp(t);
}

#define XCAM_TIMESTAMP_FORMAT "%02d:%02d:%02d.%03d"

#define XCAM_TIMESTAMP_ARGS(t)                \
    (int32_t)(XCAM_TIMESTAMP_2_SECONDS(t)/3600),       \
    (int32_t)((XCAM_TIMESTAMP_2_SECONDS(t)%3600)/60),  \
    (int32_t)(XCAM_TIMESTAMP_2_SECONDS(t)%60),         \
    (int32_t)(((t)/1000)%1000)

/* align must be an integer of power 2 */
static inline int64_t xcam_align_up(int64_t value, int64_t align)
{
    return (value + (align - 1)) & ~(align - 1);
}

static inline int64_t xcam_align_down(int64_t value, int64_t align)
{
    return value & ~(align - 1);
}

static inline int64_t xcam_align_around(int64_t value, int64_t align)
{
    const int64_t roundingDivisor = 2;
    if (align == 0) {
        return value;
    }
    return ((value + align / roundingDivisor) / align) * align;
}

static inline int64_t XCAM_ALIGN_UP(int64_t value, int64_t align)
{
    return xcam_align_up(value, align);
}

static inline int64_t XCAM_ALIGN_DOWN(int64_t value, int64_t align)
{
    return xcam_align_down(value, align);
}

static inline int64_t XCAM_ALIGN_AROUND(int64_t value, int64_t align)
{
    return xcam_align_around(value, align);
}

#ifdef _LP64
#define PRIuS "zu"
#else
#define PRIuS "u"
#endif

static inline float xcam_degree2radian(float degree)
{
    return degree * 3.1415926f / 180.0f;
}

#define PI 3.1415926f

static inline float degree2radian(float degree)
{
    return xcam_degree2radian(degree);
}

#endif //XCAM_DEFS_H
