/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define MODULE_TAG "mpi_enc_utils"

#include <cstring>
#include "securec.h"
#include "mpp_mem.h"
#include "mpp_env.h"
#include "mpp_log.h"
#include "mpp_buffer.h"
#include "rk_mpi.h"
#include "mpp_common.h"
#include "mpp_packet.h"
#include "mpi_enc_utils.h"

const int GOP_MODE_TSVC4 = 3;
const int GOP_MODE_TSVC3 = 2;
const int GOP_MODE_TSVC2 = 1;

const int TEMPORAL_ID_0 = 0;
const int TEMPORAL_ID_1 = 1;
const int TEMPORAL_ID_2 = 2;
const int TEMPORAL_ID_3 = 3;

const int REF_NUM_0 = 0;
const int REF_NUM_1 = 1;
const int REF_NUM_2 = 2;
const int REF_NUM_3 = 3;
const int REF_NUM_4 = 4;
const int REF_NUM_5 = 5;
const int REF_NUM_6 = 6;
const int REF_NUM_7 = 7;
const int REF_NUM_8 = 8;

const int ST_COUNT_3 = 3;
const int ST_COUNT_5 = 5;

void genltRefTsvc4(MppEncRefLtFrmCfg *lt_ref, MppEncRefStFrmCfg *st_ref, RK_S32 *lt_cnt, RK_S32 *st_cnt)
{
    const int REF_COUNT_9 = 9;
    *lt_cnt = 1;

    /* set 8 frame lt-ref gap */
    lt_ref[REF_NUM_0].temporal_id = TEMPORAL_ID_0;
    lt_ref[REF_NUM_0].ref_mode = REF_TO_PREV_LT_REF;
    lt_ref[REF_NUM_0].lt_gap = 8; // 8:set 8 frame

    *st_cnt = REF_COUNT_9;
    /* set tsvc4 st-ref struct */
    /* st 0 layer 0 - ref */
    st_ref[REF_NUM_0].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_0].ref_mode = REF_TO_TEMPORAL_LAYER;
    
    /* st 1 layer 3 - non-ref */
    st_ref[REF_NUM_1].is_non_ref = 1;
    st_ref[REF_NUM_1].temporal_id = TEMPORAL_ID_3;
    st_ref[REF_NUM_1].ref_mode = REF_MODE_TO_PREV_REF_FRM;

    /* st 2 layer 2 - ref */
    st_ref[REF_NUM_2].temporal_id = TEMPORAL_ID_2;
    st_ref[REF_NUM_2].ref_mode = REF_MODE_TO_PREV_REF_FRM;

    /* st 3 layer 3 - non-ref */
    st_ref[REF_NUM_3].is_non_ref = 1;
    st_ref[REF_NUM_3].temporal_id = TEMPORAL_ID_3;
    st_ref[REF_NUM_3].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 4 layer 1 - ref */
    st_ref[REF_NUM_4].temporal_id = TEMPORAL_ID_1;
    st_ref[REF_NUM_4].ref_mode = REF_TO_PREV_LT_REF;
    
    /* st 5 layer 3 - non-ref */
    st_ref[REF_NUM_5].is_non_ref = 1;
    st_ref[REF_NUM_5].temporal_id = TEMPORAL_ID_3;
    st_ref[REF_NUM_5].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 6 layer 2 - ref */
    st_ref[REF_NUM_6].temporal_id = TEMPORAL_ID_2;
    st_ref[REF_NUM_6].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 7 layer 3 - non-ref */
    st_ref[REF_NUM_7].is_non_ref = 1;
    st_ref[REF_NUM_7].temporal_id = TEMPORAL_ID_3;
    st_ref[REF_NUM_7].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 8 layer 0 - ref */
    st_ref[REF_NUM_8].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_8].ref_mode = REF_TO_TEMPORAL_LAYER;
}

void genltRefTsvc3(MppEncRefLtFrmCfg *lt_ref, MppEncRefStFrmCfg *st_ref, RK_S32 *lt_cnt, RK_S32 *st_cnt)
{
    // tsvc3
    //     /-> P1      /-> P3
    //    /           /
    //   //--------> P2
    //  //
    // P0/---------------------> P4
    *lt_cnt = 0;

    *st_cnt = ST_COUNT_5;

    /* set tsvc4 st-ref struct */
    /* st 0 layer 0 - ref */
    st_ref[REF_NUM_0].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_0].ref_mode = REF_TO_TEMPORAL_LAYER;
    
    /* st 1 layer 2 - non-ref */
    st_ref[REF_NUM_1].is_non_ref = 1;
    st_ref[REF_NUM_1].temporal_id = TEMPORAL_ID_2;
    st_ref[REF_NUM_1].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 2 layer 1 - ref */
    st_ref[REF_NUM_2].temporal_id = TEMPORAL_ID_1;
    st_ref[REF_NUM_2].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 3 layer 2 - non-ref */
    st_ref[REF_NUM_3].is_non_ref = 1;
    st_ref[REF_NUM_3].temporal_id = TEMPORAL_ID_2;
    st_ref[REF_NUM_3].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    
    /* st 4 layer 0 - ref */
    st_ref[REF_NUM_4].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_4].ref_mode = REF_TO_TEMPORAL_LAYER;
}

void genltRefTsvc2(MppEncRefStFrmCfg *st_ref, RK_S32 *lt_cnt, RK_S32 *st_cnt)
{
    // tsvc2
    //   /-> P1
    //  /
    // P0--------> P2
    *lt_cnt = 0;

    *st_cnt = ST_COUNT_3;
    /* set tsvc4 st-ref struct */
    /* st 0 layer 0 - ref */
    st_ref[REF_NUM_0].is_non_ref = 0;
    st_ref[REF_NUM_0].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_0].ref_mode = REF_TO_TEMPORAL_LAYER;
    st_ref[REF_NUM_0].ref_arg = 0;
    st_ref[REF_NUM_0].repeat = 0;
    /* st 1 layer 2 - non-ref */
    st_ref[REF_NUM_1].is_non_ref = 1;
    st_ref[REF_NUM_1].temporal_id = TEMPORAL_ID_1;
    st_ref[REF_NUM_1].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    st_ref[REF_NUM_1].ref_arg = 0;
    st_ref[REF_NUM_1].repeat = 0;
    /* st 2 layer 1 - ref */
    st_ref[REF_NUM_2].is_non_ref = 0;
    st_ref[REF_NUM_2].temporal_id = TEMPORAL_ID_0;
    st_ref[REF_NUM_2].ref_mode = REF_MODE_TO_PREV_REF_FRM;
    st_ref[REF_NUM_2].ref_arg = 0;
    st_ref[REF_NUM_2].repeat = 0;
}

static MPP_RET mpi_enc_gen_ref_cfg(MppEncRefCfg ref, RK_S32 gop_mode)
{
    MppEncRefLtFrmCfg lt_ref[4];
    MppEncRefStFrmCfg st_ref[16];
    RK_S32 lt_cnt = 0;
    RK_S32 st_cnt = 0;
    MPP_RET ret = MPP_OK;

    if (EOK != memset_s(&lt_ref, sizeof(lt_ref), 0, sizeof(lt_ref))) {
        mpp_err("memset_s failed\n");
    }
    if (EOK != memset_s(&st_ref, sizeof(st_ref), 0, sizeof(st_ref))) {
        mpp_err("memset_s failed\n");
    }

    switch (gop_mode) {
        case GOP_MODE_TSVC4: {
            genltRefTsvc4(lt_ref, st_ref, &lt_cnt, &st_cnt);
            break;
        }
        case GOP_MODE_TSVC3: {
            genltRefTsvc3(lt_ref, st_ref, &lt_cnt, &st_cnt);
            break;
        }
        case GOP_MODE_TSVC2: {
            genltRefTsvc2(st_ref, &lt_cnt, &st_cnt);
            break;
        }
        default: {
            mpp_err_f("unsupport gop mode %d\n", gop_mode);
            break;
        }
    }

    if (lt_cnt || st_cnt) {
        ret = mpp_enc_ref_cfg_set_cfg_cnt(ref, lt_cnt, st_cnt);

        if (lt_cnt) {
            ret = mpp_enc_ref_cfg_add_lt_cfg(ref, lt_cnt, lt_ref);
        }
        if (st_cnt) {
            ret = mpp_enc_ref_cfg_add_st_cfg(ref, st_cnt, st_ref);
        }
        /* check and get dpb size */
        ret = mpp_enc_ref_cfg_check(ref);
    }

    return ret;
}

static MPP_RET mpi_enc_gen_smart_gop_ref_cfg(MppEncRefCfg ref, RK_S32 gop_len, RK_S32 vi_len)
{
    MppEncRefLtFrmCfg lt_ref[4];
    MppEncRefStFrmCfg st_ref[16];
    RK_S32 lt_cnt = 1;
    RK_S32 st_cnt = 8;
    RK_S32 pos = 0;
    MPP_RET ret = MPP_OK;
    const int MIN_REPEAT = 2;

    errno_t eok = memset_s(&lt_ref, sizeof(lt_ref), 0, sizeof(lt_ref));
    if (eok != EOK) {
        mpp_err("memset_s failed\n");
        return MPP_NOK;
    }
    eok = memset_s(&st_ref, sizeof(st_ref), 0, sizeof(st_ref));
    if (eok != EOK) {
        mpp_err("memset_s failed\n");
        return MPP_NOK;
    }

    ret = mpp_enc_ref_cfg_set_cfg_cnt(ref, lt_cnt, st_cnt);

    /* set 8 frame lt-ref gap */
    lt_ref[0].lt_idx = 0;
    lt_ref[0].temporal_id = 0;
    lt_ref[0].ref_mode = REF_TO_PREV_LT_REF;
    lt_ref[0].lt_gap = gop_len;
    lt_ref[0].lt_delay = 0;

    mpp_enc_ref_cfg_add_lt_cfg(ref, 1, lt_ref);

    /* st 0 layer 0 - ref */
    st_ref[pos].is_non_ref = 0;
    st_ref[pos].temporal_id = 0;
    st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
    st_ref[pos].ref_arg = 0;
    st_ref[pos].repeat = 0;
    pos++;

    /* st 1 layer 1 - non-ref */
    if (vi_len > 1) {
        st_ref[pos].is_non_ref = 0;
        st_ref[pos].temporal_id = 1;
        st_ref[pos].ref_mode = REF_MODE_TO_PREV_REF_FRM;
        st_ref[pos].ref_arg = 0;
        st_ref[pos].repeat = vi_len - MIN_REPEAT;
        pos++;
    }

    st_ref[pos].is_non_ref = 0;
    st_ref[pos].temporal_id = 0;
    st_ref[pos].ref_mode = REF_TO_PREV_INTRA;
    st_ref[pos].ref_arg = 0;
    st_ref[pos].repeat = 0;
    pos++;

    mpp_enc_ref_cfg_add_st_cfg(ref, pos, st_ref);

    /* check and get dpb size */
    ret = mpp_enc_ref_cfg_check(ref);

    return ret;
}

void setEncParam(MpiEncTestData *p, MpiEncTestArgs *cmd)
{
    // get paramter from cmd
    p->width = cmd->width;
    p->height = cmd->height;
    p->hor_stride = MPP_ALIGN(cmd->width, 16);  // 16:hor_stride
    p->ver_stride = MPP_ALIGN(cmd->height, 16); // 16:ver_stride
    p->fmt = cmd->format;
    p->type = cmd->type;
}

/* ���˱������õ�һЩ����������ߣ�����ͼ���ʽ����������ʽ�ȣ�����������֪��ֱ����0
 * test_mpp_enc_cfg_setup�������Զ�����Ĭ�ϵĲ�����bps�� gop�Ȳ���
 */
const int MPP_ALIGNMENT_SIZE = 64;
static MPP_RET test_ctx_init(MpiEncTestData **data, MpiEncTestArgs *cmd)
{
    MpiEncTestData *p = NULL;
    MPP_RET ret = MPP_OK;
    const int YUV420_CHANNEL_RATIO = 3 / 2;
    if (!data || !cmd) {
        mpp_err_f("invalid input data %p cmd %p\n", data, cmd);
        return MPP_ERR_NULL_PTR;
    }

    p = static_cast<MpiEncTestData*>(mpp_osal_malloc(__FUNCTION__, sizeof(MpiEncTestData)));
    if (!p) {
        mpp_err_f("create MpiEncTestData failed\n");
        *data = p;
        return ret;
    }
    setEncParam(p, cmd);
    // update resource parameter
    switch (p->fmt & MPP_FRAME_FMT_MASK) {
        case MPP_FMT_YUV420SP:
        case MPP_FMT_YUV420P: {
            p->frame_size = MPP_ALIGN(p->hor_stride, (RK_U32)MPP_ALIGNMENT_SIZE) *
                MPP_ALIGN(p->ver_stride, (RK_U32)MPP_ALIGNMENT_SIZE) * YUV420_CHANNEL_RATIO;
            break;
        }
        case MPP_FMT_YUV422_YUYV:
        case MPP_FMT_YUV422_YVYU:
        case MPP_FMT_YUV422_UYVY:
        case MPP_FMT_YUV422_VYUY:
        case MPP_FMT_YUV422P:
        case MPP_FMT_YUV422SP:
        case MPP_FMT_RGB444:
        case MPP_FMT_BGR444:
        case MPP_FMT_RGB555:
        case MPP_FMT_BGR555:
        case MPP_FMT_RGB565:
        case MPP_FMT_BGR565: {
            p->frame_size = MPP_ALIGN(p->hor_stride, (RK_U32)MPP_ALIGNMENT_SIZE) *
                MPP_ALIGN(p->ver_stride, (RK_U32)MPP_ALIGNMENT_SIZE) * BPP_RGB_16BIT_FORMATS;
            break;
        }
        default: {
            p->frame_size = MPP_ALIGN(p->hor_stride, (RK_U32)MPP_ALIGNMENT_SIZE) *
                MPP_ALIGN(p->ver_stride, (RK_U32)MPP_ALIGNMENT_SIZE) * BPP_DEFAULT_FORMATS;
            break;
        }
    }
    *data = p;
    return ret;
}

static MPP_RET test_ctx_deinit(MpiEncTestData **data)
{
    MpiEncTestData *p = NULL;

    if (!data) {
        mpp_err_f("invalid input data %p\n", data);
        return MPP_ERR_NULL_PTR;
    }
    p = *data;
    if (p) {
        mpp_osal_free(__FUNCTION__, p);
        *data = NULL;
    }
    return MPP_OK;
}

const RK_S32 DEFAULT_FPS_DEN = 1;
const RK_S32 DEFAULT_FPS_NUM = 30;
const RK_S32 DEFAULT_BPS_CALC_FACTOR = 8;
const RK_S32 DEFAULT_GOP_LEN_MULTIPLIER = 2;
const RK_S32 RC_DROP_THRESHOLD_PERCENT = 20;
const RK_S32 RC_DROP_GAP = 1;

const RK_S32 BPS_MAX_FACTOR_NUMERATOR = 17;
const RK_S32 BPS_MAX_FACTOR_DENOMINATOR = 16;
const RK_S32 BPS_MIN_CBR_FACTOR_NUMERATOR = 15;
const RK_S32 BPS_MIN_CBR_FACTOR_DENOMINATOR = 16;
const RK_S32 BPS_MIN_VBR_FACTOR_NUMERATOR = 1;
const RK_S32 BPS_MIN_VBR_FACTOR_DENOMINATOR = 16;

const RK_S32 FIX_QP_VALUE = 20;
const RK_S32 RC_QP_INIT = 26;
const RK_S32 RC_QP_MAX = 51;
const RK_S32 RC_QP_MIN = 10;
const RK_S32 RC_QP_MAX_I = 51;
const RK_S32 RC_QP_MIN_I = 10;
const RK_S32 RC_QP_IP = 2;

const RK_S32 VP8_QP_INIT = 40;
const RK_S32 VP8_QP_MAX = 127;
const RK_S32 VP8_QP_MIN = 0;
const RK_S32 VP8_QP_MAX_I = 127;
const RK_S32 VP8_QP_MIN_I = 0;
const RK_S32 VP8_QP_IP = 6;

const RK_S32 JPEG_Q_FACTOR = 80;
const RK_S32 JPEG_QF_MAX = 99;
const RK_S32 JPEG_QF_MIN = 1;

const RK_S32 H264_PROFILE_HIGH = 100;
const RK_S32 H264_LEVEL_40 = 40;
const RK_S32 H264_CABAC_ENABLED = 1;
const RK_S32 H264_CABAC_IDC = 0;
const RK_S32 H264_TRANS8x8_ENABLED = 1;
const RK_S32 GOP_MODE_THRESHOLD = 4;

void setDefaultMpiEncPatams(MpiEncTestData *p)
{
    /* setup default parameter */
    if (p->fps_in_den == 0) {
        p->fps_in_den = DEFAULT_FPS_DEN;
    }
    if (p->fps_in_num == 0) {
        p->fps_in_num = DEFAULT_FPS_NUM;
    }
    if (p->fps_out_den == 0) {
        p->fps_out_den = DEFAULT_FPS_DEN;
    }
    if (p->fps_out_num == 0) {
        p->fps_out_num = DEFAULT_FPS_NUM;
    }

    if (!p->bps) {
        p->bps = p->width * p->height / DEFAULT_BPS_CALC_FACTOR * (p->fps_out_num / p->fps_out_den);
    }

    p->split_mode = 0;
    p->split_arg = 0;
}

void setCfgBpsByRcMode(MpiEncTestData *p, MppEncCfg *cfg)
{
    switch (p->rc_mode) {
        case MPP_ENC_RC_MODE_FIXQP: {
            /* do not setup bitrate on FIXQP mode */
            break;
        }
        case MPP_ENC_RC_MODE_CBR: {
            /* CBR mode has narrow bound */
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_max",
                p->bps_max ? p->bps_max : p->bps * BPS_MAX_FACTOR_NUMERATOR / BPS_MAX_FACTOR_DENOMINATOR);
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_min",
                p->bps_min ? p->bps_min : p->bps * BPS_MIN_CBR_FACTOR_NUMERATOR / BPS_MIN_CBR_FACTOR_DENOMINATOR);
            break;
        }
        case MPP_ENC_RC_MODE_VBR:
        case MPP_ENC_RC_MODE_AVBR: {
            /* VBR mode has wide bound */
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_max",
                p->bps_max ? p->bps_max : p->bps * BPS_MAX_FACTOR_NUMERATOR / BPS_MAX_FACTOR_DENOMINATOR);
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_min",
                p->bps_min ? p->bps_min : p->bps * BPS_MIN_VBR_FACTOR_NUMERATOR / BPS_MIN_VBR_FACTOR_DENOMINATOR);
            break;
        }
        default: {
            /* default use CBR mode */
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_max",
                p->bps_max ? p->bps_max : p->bps * BPS_MAX_FACTOR_NUMERATOR / BPS_MAX_FACTOR_DENOMINATOR);
            mpp_enc_cfg_set_s32(*cfg, "rc:bps_min",
                p->bps_min ? p->bps_min : p->bps * BPS_MIN_CBR_FACTOR_NUMERATOR / BPS_MIN_CBR_FACTOR_DENOMINATOR);
            break;
        }
    }
}

void setCfgQpByRcMode(MpiEncTestData *p, MppEncCfg *cfg)
{
    switch (p->rc_mode) {
        case MPP_ENC_RC_MODE_FIXQP: {
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_init", FIX_QP_VALUE);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max", FIX_QP_VALUE);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min", FIX_QP_VALUE);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max_i", FIX_QP_VALUE);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min_i", FIX_QP_VALUE);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_ip", RC_QP_IP);
            break;
        }
        case MPP_ENC_RC_MODE_CBR:
        case MPP_ENC_RC_MODE_VBR:
        case MPP_ENC_RC_MODE_AVBR: {
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_init", RC_QP_INIT);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max", RC_QP_MAX);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min", RC_QP_MIN);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max_i", RC_QP_MAX_I);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min_i", RC_QP_MIN_I);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_ip", RC_QP_IP);
            break;
        }
        default: {
            mpp_err_f("unsupport encoder rc mode %d\n", p->rc_mode);
            break;
        }
    }
}

void setCfgQpByType(MpiEncTestData *p, MppEncCfg *cfg)
{
    /* setup qp for different codec and rc_mode */
    switch (p->type) {
        case MPP_VIDEO_CodingAVC:
        case MPP_VIDEO_CodingHEVC: {
            setCfgQpByRcMode(p, cfg);
            break;
        }
        case MPP_VIDEO_CodingVP8: {
            /* vp8 only setup base qp range */
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_init", VP8_QP_INIT);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max", VP8_QP_MAX);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min", VP8_QP_MIN);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_max_i", VP8_QP_MAX_I);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_min_i", VP8_QP_MIN_I);
            mpp_enc_cfg_set_s32(*cfg, "rc:qp_ip", VP8_QP_IP);
            break;
        }
        case MPP_VIDEO_CodingMJPEG: {
            /* jpeg use special codec config to control qtable */
            mpp_enc_cfg_set_s32(*cfg, "jpeg:q_factor", JPEG_Q_FACTOR);
            mpp_enc_cfg_set_s32(*cfg, "jpeg:qf_max", JPEG_QF_MAX);
            mpp_enc_cfg_set_s32(*cfg, "jpeg:qf_min", JPEG_QF_MIN);
            break;
        }
        default: {
            break;
        }
    }
}

void setCfgCodecByType(MpiEncTestData *p, MppEncCfg *cfg)
{
    switch (p->type) {
        case MPP_VIDEO_CodingAVC: {
            /*
             * H.264 profile_idc parameter
             * 66  - Baseline profile
             * 77  - Main profile
             * 100 - High profile
             */
            mpp_enc_cfg_set_s32(*cfg, "h264:profile", H264_PROFILE_HIGH);
            /*
             * H.264 level_idc parameter
             * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
             * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
             * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
             * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
             * 50 / 51 / 52         - 4K@30fps
             */
            mpp_enc_cfg_set_s32(*cfg, "h264:level", H264_LEVEL_40);
            mpp_enc_cfg_set_s32(*cfg, "h264:cabac_en", H264_CABAC_ENABLED);
            mpp_enc_cfg_set_s32(*cfg, "h264:cabac_idc", H264_CABAC_IDC);
            mpp_enc_cfg_set_s32(*cfg, "h264:trans8x8", H264_TRANS8x8_ENABLED);
            break;
        }
        case MPP_VIDEO_CodingHEVC:
        case MPP_VIDEO_CodingMJPEG:
        case MPP_VIDEO_CodingVP8: {
            break;
        }
        default: {
            mpp_err_f("unsupport encoder coding type %d\n", p->type);
            break;
        }
    }
}

MPP_RET setCfgToMpp(MppCtx ctx, MpiEncTestData *p, MppEncCfg *cfg)
{
    MPP_RET ret = MPP_OK;

    setDefaultMpiEncPatams(p);
    mpp_enc_cfg_set_s32(*cfg, "prep:width", p->width);
    mpp_enc_cfg_set_s32(*cfg, "prep:height", p->height);
    mpp_enc_cfg_set_s32(*cfg, "prep:hor_stride", p->hor_stride);
    mpp_enc_cfg_set_s32(*cfg, "prep:ver_stride", p->ver_stride);
    mpp_enc_cfg_set_s32(*cfg, "prep:format", p->fmt);

    mpp_enc_cfg_set_s32(*cfg, "rc:mode", p->rc_mode);

    /* fix input / output frame rate */
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_in_flex", p->fps_in_flex);
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_in_num", p->fps_in_num);
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_in_denorm", p->fps_in_den);
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_out_flex", p->fps_out_flex);
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_out_num", p->fps_out_num);
    mpp_enc_cfg_set_s32(*cfg, "rc:fps_out_denorm", p->fps_out_den);
    mpp_enc_cfg_set_s32(*cfg, "rc:gop", p->gop_len ? p->gop_len : p->fps_out_num * DEFAULT_GOP_LEN_MULTIPLIER);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(*cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(*cfg, "rc:drop_thd", RC_DROP_THRESHOLD_PERCENT); /* 20% of max bps */
    mpp_enc_cfg_set_u32(*cfg, "rc:drop_gap", RC_DROP_GAP);               /* Do not continuous drop frame */

    /* setup bitrate for different rc_mode */
    mpp_enc_cfg_set_s32(*cfg, "rc:bps_target", p->bps);
    setCfgBpsByRcMode(p, cfg);
    setCfgQpByType(p, cfg);

    /* setup codec  */
    mpp_enc_cfg_set_s32(cfg, "codec:type", p->type);
    setCfgCodecByType(p, cfg);

    mpp_env_get_u32("split_mode", &p->split_mode, MPP_ENC_SPLIT_NONE);
    mpp_env_get_u32("split_arg", &p->split_arg, 0);

    if (p->split_mode) {
        mpp_log("%p split_mode %d split_arg %d\n", ctx, p->split_mode, p->split_arg);
        mpp_enc_cfg_set_s32(*cfg, "split:mode", p->split_mode);
        mpp_enc_cfg_set_s32(*cfg, "split:arg", p->split_arg);
    }

    ret = p->mpi->control(ctx, MPP_ENC_SET_CFG, *cfg);
    if (ret) {
        mpp_err("mpi control enc set cfg failed ret %d\n", ret);
        return ret;
    }
    return MPP_OK;
}

static MPP_RET test_mpp_enc_cfg_setup(MpiEncTestData *p)
{
    MPP_RET ret = MPP_OK;
    MppApi *mpi;
    MppCtx ctx;
    MppEncCfg cfg;

    if (p == NULL) {
        return MPP_ERR_NULL_PTR;
    }
    mpi = p->mpi;
    ctx = p->ctx;
    cfg = p->cfg;

    ret = setCfgToMpp(ctx, p, &cfg);
    if (ret != MPP_OK) {
        return ret;
    }

    /* optional */
    p->sei_mode = MPP_ENC_SEI_MODE_ONE_FRAME;
    ret = mpi->control(ctx, MPP_ENC_SET_SEI_CFG, &p->sei_mode);
    if (ret) {
        mpp_err("mpi control enc set sei cfg failed ret %d\n", ret);
        return ret;
    }

    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        p->header_mode = MPP_ENC_HEADER_MODE_EACH_IDR;
        ret = mpi->control(ctx, MPP_ENC_SET_HEADER_MODE, &p->header_mode);
        if (ret) {
            mpp_err("mpi control enc set header mode failed ret %d\n", ret);
            return ret;
        }
    }

    RK_U32 gop_mode = p->gop_mode;

    mpp_env_get_u32("gop_mode", &gop_mode, gop_mode);
    if (gop_mode) {
        MppEncRefCfg ref;

        mpp_enc_ref_cfg_init(&ref);

        if (p->gop_mode < GOP_MODE_THRESHOLD) {
            mpi_enc_gen_ref_cfg(ref, gop_mode);
        } else {
            mpi_enc_gen_smart_gop_ref_cfg(ref, p->gop_len, p->vi_len);
        }
        ret = mpi->control(ctx, MPP_ENC_SET_REF_CFG, ref);
        if (ret) {
            mpp_err("mpi control enc set ref cfg failed ret %d\n", ret);
            return ret;
        }
        mpp_enc_ref_cfg_deinit(&ref);
    }
    return ret;
}

static void test_mpp_frame_set_param(MppFrame frame, MpiEncTestData *p)
{
    mpp_frame_set_width(frame, p->width);
    mpp_frame_set_height(frame, p->height);
    mpp_frame_set_hor_stride(frame, p->hor_stride);
    mpp_frame_set_ver_stride(frame, p->ver_stride);
    mpp_frame_set_fmt(frame, p->fmt);
    mpp_frame_set_eos(frame, p->frm_eos);
}

static void test_mpp_ctx_cleanup(MpiEncTestData *p)
{
    if (!p) {
        mpp_err("test_mpp_ctx_cleanup p == NULL\n");
        return;
    }

    if (p->mpi->reset) {
        p->mpi->reset(p->ctx);
    }

    if (p->ctx) {
        mpp_destroy(p->ctx);
        p->ctx = NULL;
    }

    if (p->cfg) {
        mpp_enc_cfg_deinit(p->cfg);
        p->cfg = NULL;
    }

    if (p->pkt_buf) {
        mpp_buffer_put(p->pkt_buf);
        p->pkt_buf = NULL;
    }

    if (p->buf_grp) {
        mpp_buffer_group_put(p->buf_grp);
        p->buf_grp = NULL;
    }

    test_ctx_deinit(&p);
}

int hal_mpp_get_sps(void *ctx, unsigned char *buf, size_t *buf_size)
{
    int ret;
    MppApi *mpi;
    MppCtx mpp_ctx;
    MppPacket packet = NULL;
    MpiEncTestData *p = (MpiEncTestData *)ctx;
    errno_t eok;

    if (!p) {
        mpp_err("mpi control enc get extra info failed\n");
        return MPP_NOK;
    }
    mpi = p->mpi;
    mpp_ctx = p->ctx;

    /*
     * Can use packet with normal malloc buffer as input not pkt_buf.
     * Please refer to vpu_api_legacy.cpp for normal buffer case.
     * Using pkt_buf buffer here is just for simplifing demo.
     */

    mpp_packet_init_with_buffer(&packet, p->pkt_buf);
    /* NOTE: It is important to clear output packet length!! */
    mpp_packet_set_length(packet, 0);

    // ����һ��H264��H265ר�õ�һЩ����
    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        ret = mpi->control(mpp_ctx, MPP_ENC_GET_HDR_SYNC, packet);
        if (ret) {
            mpp_err("mpi control enc get extra info failed\n");
            ret = MPP_NOK;
            mpp_packet_deinit(&packet);
            return ret;
        }
    }

    void *ptr = mpp_packet_get_pos(packet);
    size_t len = mpp_packet_get_length(packet);
    if (*buf_size < len) {
        mpp_err("mpi buffer size too small\n");
        ret = MPP_NOK;
        mpp_packet_deinit(&packet);
        return ret;
    }

    eok = memcpy_s(buf, len, ptr, len);
    if (eok != EOK) {
        mpp_err("memcpy_s failed\n");
        return MPP_NOK;
    }

    *buf_size = len;

    ret = MPP_OK;
    mpp_packet_deinit(&packet);
    return ret;
}

const int INDEX_SHIFT = 27;
MPP_RET encodePutFrame(MpiEncTestData *p, MppBuffer *cam_buf, MppPacket *packet, int dma_fd)
{
    MPP_RET ret = MPP_OK;
    MppBufferInfo info;
    MppMeta meta = NULL;
    MppFrame frame = NULL;

    if (EOK != memset_s(&info, sizeof(MppBufferInfo), 0, sizeof(MppBufferInfo))) {
        mpp_err("memset_s failed\n");
    }
    info.type = MPP_BUFFER_TYPE_EXT_DMA;
    info.fd = dma_fd;
    info.size = p->frame_size & 0x07ffffff;
    info.index = (p->frame_size & 0xf8000000) >> INDEX_SHIFT;

    ret = mpp_buffer_import(cam_buf, &info);
    if (ret != MPP_SUCCESS) {
        mpp_err_f("mpp_buffer_import failed\n");
        return MPP_NOK;
    }

    ret = mpp_frame_init(&frame);
    if (ret) {
        mpp_err_f("mpp_frame_init failed\n");
        return MPP_NOK;
    }

    /* set frame size info */
    test_mpp_frame_set_param(frame, p);

    /* set frame data include fd */
    mpp_frame_set_buffer(frame, *cam_buf);

    /* packet init */
    mpp_packet_init_with_buffer(packet, p->pkt_buf);
    /* NOTE: It is important to clear output packet length!! */
    mpp_packet_set_length(*packet, 0);

    meta = mpp_frame_get_meta(frame);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, *packet);

    /*
     * NOTE: in non-block mode the frame can be resent.
     * The default input timeout mode is block.
     *
     * User should release the input frame to meet the requirements of
     * resource creator must be the resource destroyer.
     */
    ret = p->mpi->encode_put_frame(p->ctx, frame);
    if (ret) {
        mpp_err("mpp encode put frame failed\n");
        mpp_frame_deinit(&frame);
        if (*cam_buf) {
            mpp_buffer_put(*cam_buf);
        }
        mpp_packet_deinit(packet);
        return ret;
    }

    mpp_frame_deinit(&frame);
    return MPP_OK;
}

MPP_RET encodeGetPacket(MpiEncTestData *p, MppBuffer *cam_buf, MppPacket *packet)
{
    MPP_RET ret = MPP_OK;
    RK_U32 eoi = 1;
    RK_U32 packet_num = 0;

    do {
        ret = p->mpi->encode_get_packet(p->ctx, packet);
        if (ret) {
            mpp_err("mpp encode get packet failed\n");
            if (*cam_buf) {
                mpp_buffer_put(*cam_buf);
            }
            mpp_packet_deinit(packet);
            return ret;
        }

        mpp_assert(*packet);

        if (*packet) {
            /* for low delay partition encoding */
            if (mpp_packet_is_partition(*packet)) {
                eoi = mpp_packet_is_eoi(*packet);
            }

            p->frame_count += eoi;
            packet_num++;
        }
    } while (!eoi);

    if (packet_num != 1) {
        mpp_err("packet_num %u != 1\n");
        if (cam_buf) {
            mpp_buffer_put(cam_buf);
        }
        mpp_packet_deinit(packet);
        return MPP_NOK;
    }
    return MPP_OK;
}

int hal_mpp_encode(void *ctx, int dma_fd, unsigned char *buf, size_t *buf_size)
{
    if (!ctx) {
        mpp_err("memset_s failed\n");
        return MPP_NOK;
    }

    MppPacket packet = NULL;
    MpiEncTestData *p = (MpiEncTestData *)ctx;
    MppBuffer cam_buf = NULL;

    MPP_RET ret = encodePutFrame(p, &cam_buf, &packet, dma_fd);
    if (ret != MPP_OK) {
        return ret;
    }
    ret = encodeGetPacket(p, &cam_buf, &packet);
    if (ret != MPP_OK) {
        return ret;
    }

    void *ptr = mpp_packet_get_pos(packet);
    if (ptr == nullptr) {
        mpp_err("mpp_packet_get_pos failed\n");
        return MPP_NOK;
    }
    size_t len = mpp_packet_get_length(packet);
    if (*buf_size < len) {
        mpp_err("mpi buffer size too small\n");
        if (cam_buf) {
            mpp_buffer_put(cam_buf);
        }
        mpp_packet_deinit(&packet);
        return MPP_NOK;
    }

    if (EOK != memcpy_s(buf, len, ptr, len)) {
        mpp_err("memcpy_s failed\n");
    }

    *buf_size = len;
    if (cam_buf) {
        mpp_buffer_put(cam_buf);
    }
    mpp_packet_deinit(&packet);
    return MPP_OK;
}

void *hal_mpp_ctx_create(MpiEncTestArgs *args)
{
    MPP_RET ret = MPP_OK;
    MpiEncTestData *p = NULL;
    MppPollType timeout = MPP_POLL_BLOCK;

    /* ��ʼ��һ�³��õĲ������ã�cmd�����ñȽ϶� */
    ret = test_ctx_init(&p, args);
    if (ret) {
        mpp_err_f("test data init failed ret %d\n");
        return nullptr;
    }

    ret = mpp_buffer_group_get_internal(&p->buf_grp, MPP_BUFFER_TYPE_DRM);
    if (ret) {
        mpp_err_f("failed to get mpp buffer group ret %d\n", ret);
        return nullptr;
    }

    /* pkt_buf�����Ա���?���Ǹ�packetʹ�õ�buffer */
    ret = mpp_buffer_get(p->buf_grp, &p->pkt_buf, p->frame_size);
    if (ret) {
        mpp_err_f("failed to get buffer for output packet ret %d\n", ret);
        return nullptr;
    }

    // encoder demo
    ret = mpp_create(&p->ctx, &p->mpi);
    if (ret) {
        mpp_err("mpp_create failed ret %d\n", ret);
        return nullptr;
    }

    mpp_log("%p mpi_enc_test encoder test start w %d h %d type %d\n", p->ctx, p->width, p->height, p->type);

    ret = p->mpi->control(p->ctx, MPP_SET_OUTPUT_TIMEOUT, &timeout);
    if (MPP_OK != ret) {
        mpp_err("mpi control set output timeout %d ret %d\n", timeout, ret);
        return nullptr;
    }

    ret = mpp_init(p->ctx, MPP_CTX_ENC, p->type);
    if (ret) {
        mpp_err("mpp_init failed ret %d\n", ret);
        return nullptr;
    }

    ret = mpp_enc_cfg_init(&p->cfg);
    if (ret) {
        mpp_err_f("mpp_enc_cfg_init failed ret %d\n", ret);
        return nullptr;
    }

    ret = test_mpp_enc_cfg_setup(p);
    if (ret) {
        mpp_err_f("test mpp setup failed ret %d\n", ret);
        return nullptr;
    }

    return p;
}

void hal_mpp_ctx_delete(void *ctx)
{
    test_mpp_ctx_cleanup((MpiEncTestData*)ctx);
}
