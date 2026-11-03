/*
 * rk_aiq_types_adehaze_stat.h
 *
 *  Copyright (c) 2019 Rockchip Corporation
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
 */
#ifndef __RK_AIQ_TYPES_ADEHAZE_ALGO_STAT_H__
#define __RK_AIQ_TYPES_ADEHAZE_ALGO_STAT_H__

typedef struct dehaze_stats_v14_s {
    unsigned short adp_wt;
    unsigned short adp_air_base;
    unsigned short adp_tmax;
    unsigned short hist_iir[80][16];
} dehaze_stats_v14_t;

typedef struct dehaze_stats_v12_s {
    unsigned int dhaz_pic_sumh;
    unsigned short dhaz_adp_air_base;
    unsigned short dhaz_adp_wt;
    unsigned short dhaz_adp_gratio;
    unsigned short dhaz_adp_tmax;
    unsigned short h_rgb_iir[64];
} dehaze_stats_v12_t;

typedef struct dehaze_stats_v11_duo_s {
    unsigned int dhaz_pic_sumh_left;
    unsigned int dhaz_pic_sumh_right;
    int dhaz_adp_air_base;
    int dhaz_adp_wt;
    int dhaz_adp_gratio;
    int dhaz_adp_tmax;
    int h_rgb_iir[64];
} dehaze_stats_v11_duo_t;

typedef struct dehaze_stats_v11_s {
    int dhaz_adp_air_base;
    int dhaz_adp_wt;
    int dhaz_adp_gratio;
    int dhaz_adp_tmax;
    int h_rgb_iir[64];
} dehaze_stats_v11_t;

typedef struct dehaze_stats_v10_s {
    int dhaz_adp_air_base;
    int dhaz_adp_wt;
    int dhaz_adp_gratio;
    int dhaz_adp_tmax;
    int h_r_iir[64];
    int h_g_iir[64];
    int h_b_iir[64];
} dehaze_stats_v10_t;

typedef struct rkisp_adehaze_stats_s {
    bool stats_true;
#if RKAIQ_HAVE_DEHAZE_V10
    dehaze_stats_v10_t dehaze_stats_v10;
#endif
#if RKAIQ_HAVE_DEHAZE_V11
    dehaze_stats_v11_t dehaze_stats_v11;
#endif
#if RKAIQ_HAVE_DEHAZE_V11_DUO
    dehaze_stats_v11_duo_t dehaze_stats_v11_duo;
#endif
#if RKAIQ_HAVE_DEHAZE_V12
    dehaze_stats_v12_t dehaze_stats_v12;
#endif
#if RKAIQ_HAVE_DEHAZE_V14
    dehaze_stats_v14_t dehaze_stats_v14;
#endif
} rkisp_adehaze_stats_t;


#endif
