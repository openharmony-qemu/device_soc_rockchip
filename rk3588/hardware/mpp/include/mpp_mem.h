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

#ifndef __MPP_MEM_H__
#define __MPP_MEM_H__

#include <stdlib.h>

#include "rk_type.h"
#include "mpp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void *mpp_osal_malloc(const char *caller, size_t size);
void *mpp_osal_calloc(const char *caller, size_t size);
void *mpp_osal_realloc(const char *caller, void *ptr, size_t size);
void mpp_osal_free(const char *caller, void *ptr);

void mpp_show_mem_status(void);
RK_U32 mpp_mem_total_now(void);
RK_U32 mpp_mem_total_max(void);

/*
 * mpp memory usage snapshot tool
 *
 * usage:
 * call mpp_mem_get_snapshot on context init get one snapshot
 * call mpp_mem_get_snapshot on context deinit get another snapshot
 * call mpp_mem_diff_snapshot to show the difference between these two snapshot
 * call mpp_mem_put_snapshot twice to release these two snapshot
 */
typedef void* MppMemSnapshot;

MPP_RET mpp_mem_get_snapshot(MppMemSnapshot *hnd);
MPP_RET mpp_mem_put_snapshot(MppMemSnapshot *hnd);
MPP_RET mpp_mem_squash_snapshot(MppMemSnapshot hnd0, MppMemSnapshot hnd1);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_MEM_H__*/

