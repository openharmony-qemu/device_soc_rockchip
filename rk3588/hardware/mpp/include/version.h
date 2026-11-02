/*
 * Copyright (C) 2026 Rockchip Electronics Co., Ltd.
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

#ifndef __VERSION_H__
#define __VERSION_H__

#define MPP_VERSION             "b35ca00 author: innov_open     2026-08-06 build: add build_version.sh and generate" \
                                 " version.h via GN action"
#define MPP_VER_HIST_CNT        10
#define MPP_VER_HIST_0          "b35ca00 author: innov_open     2026-08-06 build: add build_version.sh and generate" \
                                 " version.h via GN action  (HEAD, openfj/develop, m/OpenHarmony-6.1-LTS)"
#define MPP_VER_HIST_1          "6c6bfbc author: innov_open     2026-07-31 Add README file "
#define MPP_VER_HIST_2          "6bba389 author: innov_open     2026-07-10 mpp: include vepu51x_common.c and switch" \
                                 " part_name to soc_rockchip "
#define MPP_VER_HIST_3          "3555805 author: innov_open     2026-06-30 Add OpenHarmony build files and resolve" \
                                 " compile issues "
#define MPP_VER_HIST_4          "238458d author: Hongjin Li     2026-06-26 feat[av1d]: Store order_hint via mpp_frame" \
                                 " poc field "
#define MPP_VER_HIST_5          "9b79bc0 author: Hongjin Li     2026-06-26 feat[hal_dbg]: Support appending data load" \
                                 " across multiple calls "
#define MPP_VER_HIST_6          "38a0f80 author: Hongjin Li     2026-06-26 feat[hal_dbg]: Disable hal_dbg in fast mode "
#define MPP_VER_HIST_7          "482a859 author: timkingh.huang 2026-06-26 fix[vepu511a]: Distinguish between" \
                                 " qpmap_en and deblur_en "
#define MPP_VER_HIST_8          "37fc589 author: Yandong Lin    2026-06-25 fix[kmpp_obj]: Clear ioc entry when" \
                                 " putting ioc obj "
#define MPP_VER_HIST_9          "f824bff author: Herman Chen    2026-06-24 chore[kmpp_obj]: Add shm resize implement " \
                                 " (grafted)"

#endif /*__VERSION_H__*/
