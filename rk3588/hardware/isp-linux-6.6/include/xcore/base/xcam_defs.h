/* SPDX-License-Identifier: Apache-2.0 */
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

#include <stdint.h>
#include <stdlib.h>

#if defined(__linux__)
#include <sys/time.h>
#include <time.h>
#include "unistd.h"
#endif

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

static inline const char *xcam_str(const char *str)
{
    return str ? str : "null";
}

static inline const char *xcam_bool2str(int value)
{
    return value ? "true" : "false";
}

static inline int xcam_double_equal(double a, double b, double tolerance)
{
    return (a >= (b - tolerance)) && (a <= (b + tolerance));
}

#define XCAM_DOUBLE_EQUAL_EPSILON 0.000001

static inline int xcam_double_equal_around(double a, double b)
{
    return xcam_double_equal(a, b, XCAM_DOUBLE_EQUAL_EPSILON);
}

#define XCAM_GAMMA_TABLE_SIZE 256
#define XCAM_MAX_STR_SIZE 4096
#undef XCAM_CL_MAX_STR_SIZE
#define XCAM_CL_MAX_STR_SIZE 1024

static inline int64_t xcam_timespec_2_nsec(struct timespec ts)
{
    return (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

#define XCAM_NSEC_PER_USEC 1000

#define XCAM_USEC_PER_SEC 1000000

static inline int64_t xcam_timespec_2_usec(struct timespec ts)
{
    return (int64_t)ts.tv_sec * XCAM_USEC_PER_SEC + ts.tv_nsec / XCAM_NSEC_PER_USEC;
}

static inline int64_t xcam_timeval_2_usec(struct timeval tv)
{
    return (int64_t)tv.tv_sec * XCAM_USEC_PER_SEC + tv.tv_usec;
}

static inline int64_t xcam_timestamp_2_seconds(int64_t t)
{
    return t / XCAM_USEC_PER_SEC;
}

static inline int64_t xcam_seconds_2_timestamp(int64_t sec)
{
    return sec * XCAM_USEC_PER_SEC;
}

#define XCAM_TIMESTAMP_FORMAT "%02d:%02d:%02d.%03d"

/* align must be an integer of power 2 */
static inline int64_t xcam_align_up(int64_t value, int64_t align)
{
    return (value + (align - 1)) & ~(align - 1);
}

static inline int64_t xcam_align_down(int64_t value, int64_t align)
{
    return value & ~(align - 1);
}

#define XCAM_ALIGN_HALF 2

static inline int64_t xcam_align_around(int64_t value, int64_t align)
{
    if (align == 0) {
        return value;
    }
    return ((value + align / XCAM_ALIGN_HALF) / align) * align;
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

#endif /* XCAM_DEFS_H */
