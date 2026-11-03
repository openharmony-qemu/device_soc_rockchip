/*
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

#ifndef _RK_AIQ_TYPES_H_
#define _RK_AIQ_TYPES_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "common/rk_aiq_comm.h"
#include "common/rk_aiq_mems_sensor.h"
#include "common/rk-camera-module.h"
#include "algos/adebayer/rk_aiq_types_adebayer_algo.h"
#include "algos/ae/rk_aiq_types_ae_algo.h"
#include "algos/afd/rk_aiq_types_afd_algo.h"
#include "algos/awb/rk_aiq_types_awb_algo.h"
#include "algos/alsc/rk_aiq_types_alsc_algo.h"
#include "algos/accm/rk_aiq_types_accm_algo.h"
#include "algos/a3dlut/rk_aiq_types_a3dlut_algo.h"
#include "algos/amerge/rk_aiq_types_amerge_algo_int.h"
#include "algos/atmo/rk_aiq_types_atmo_algo_int.h"
#include "algos/atmo/rk_aiq_types_atmo_algo.h"
#include "algos/adrc/rk_aiq_types_adrc_algo_int.h"
#include "algos/adrc/rk_aiq_types_adrc_algo.h"
#include "algos/agamma/rk_aiq_types_agamma_algo_int.h"
#include "algos/adegamma/rk_aiq_types_adegamma_algo_int.h"
#include "algos/adehaze/rk_aiq_types_adehaze_algo_int.h"
#include "algos/adehaze/rk_aiq_types_adehaze_algo.h"
#include "algos/acp/rk_aiq_types_acp_algo.h"
#include "algos/aie/rk_aiq_types_aie_algo_int.h"
#include "algos/aorb/rk_aiq_types_orb_algo.h"
#include "algos/asd/rk_aiq_types_asd_algo.h"
#include "algos/anr/rk_aiq_types_anr_algo.h"
#include "algos/anr/rk_aiq_types_anr_algo_int.h"
#include "algos/asharp/rk_aiq_types_asharp_algo.h"
#include "algos/asharp/rk_aiq_types_asharp_algo_int.h"
#include "algos/adpcc/rk_aiq_types_adpcc_algo.h"
#include "algos/adpcc/rk_aiq_types_adpcc_algo_int.h"
#include "algos/ablc/rk_aiq_types_ablc_algo.h"
#include "algos/ablc/rk_aiq_types_ablc_algo_int.h"
#include "algos/agic/rk_aiq_types_algo_agic_int.h"
#include "algos/aldch/rk_aiq_types_aldch_algo.h"
#include "algos/afec/rk_aiq_types_afec_algo.h"
#include "algos/asharp3/rk_aiq_types_asharp_algo_v3.h"
#include "algos/aynr2/rk_aiq_types_aynr_algo_v2.h"
#include "algos/acnr/rk_aiq_types_acnr_algo_v1.h"
#include "algos/arawnr2/rk_aiq_types_abayernr_algo_v2.h"
#include "algos/amd/rk_aiq_types_amd_algo.h"
#include "algos/arawnr/rk_aiq_types_abayernr_algo_v1.h"
#include "algos/amfnr/rk_aiq_types_amfnr_algo_v1.h"
#include "algos/aynr/rk_aiq_types_aynr_algo_v1.h"
#include "algos/auvnr/rk_aiq_types_auvnr_algo_v1.h"
#include "algos/again/rk_aiq_types_again_algo.h"
#include "algos/acsm/rk_aiq_types_acsm_algo.h"
#include "algos/acgc/rk_aiq_types_acgc_algo.h"

#ifdef RK_SIMULATOR_HW
#include "simulator/isp20_hw_simulator.h"
#include "af/rk_aiq_af_hw_v200.h"
#else
#include "algos/af/rk_aiq_types_af_algo.h"

#endif

#include "common/rk_aiq_types_v20.h"
#include "common/rk_aiq_types_v21.h"
#include "common/rk_aiq_types_v3x.h" /*< v3x types */
#include "common/rk_aiq_types_v32.h"
#include "common/rk_aiq_types_v39.h"

#if ISP_HW_V39
#include "isp/rk_aiq_stats_awb39.h"
#elif ISP_HW_V33
#include "isp/rk_aiq_stats_awb33.h"
#endif

#define ANR_NO_SEPERATE_MARCO (0)

typedef struct rk_aiq_color_info_s {
    float sensorGain;
    float awbGain[2];
} rk_aiq_color_info_t;

typedef struct rk_aiq_range_s {
    float min;
    float max;
    float step;
} rk_aiq_range_t;

typedef struct rk_aiq_rect_s {
    int left;
    int top;
    int width;
    int height;
} rk_aiq_rect_t;

typedef enum rk_aiq_rotation_e { RK_AIQ_ROTATION_0, RK_AIQ_ROTATION_90, RK_AIQ_ROTATION_270 } rk_aiq_rotation_t;

typedef enum rk_aiq_af_sec_stat_e {
    RK_AIQ_AF_SEARCH_INVAL = 0,
    RK_AIQ_AF_SEARCH_RUNNING = 1,
    RK_AIQ_AF_SEARCH_END = 2,
    RK_AIQ_AF_ZOOM_MOVING = 3,
    RK_AIQ_AF_ZOOM_MOVE_END = 4
} rk_aiq_af_sec_stat_t;

#include "common/rk_aiq_pixel_format.h"

typedef struct rk_frame_fmt_s {
    int32_t width;
    int32_t height;
    rk_aiq_format_t format;
    int32_t fps;
    int32_t hdr_mode;
} rk_frame_fmt_t;

#define SUPPORT_FMT_MAX 10
typedef struct {
    char sensor_name[32];
    rk_frame_fmt_t support_fmt[SUPPORT_FMT_MAX];
    int32_t num;
    /* binded pp stream media index */
    int8_t binded_strm_media_idx;
    int phyId;
} rk_aiq_sensor_info_t;

typedef struct {
    char len_name[32];
} rk_aiq_lens_info_t;

typedef struct {
    rk_aiq_sensor_info_t sensor_info;
    rk_aiq_lens_info_t lens_info;
    int isp_hw_ver;
    bool has_lens_vcm; /*< has lens vcm */
    bool has_fl;       /*< has flash light */
    bool fl_strth_adj_sup;
    bool has_irc; /*< has ircutter */
    bool fl_ir_strth_adj_sup;
    bool _is_1608_sensor; /*< for sensor sync mode verify. */
    uint8_t is_multi_isp_mode;
    uint16_t multi_isp_extended_pixel;
    uint8_t reserved[13];
    // supported Antibanding modes
    // supported lock modes
    // supported ae compensation range/step
    // supported ae measure mode
    // supported af modes
    // other from iq
} rk_aiq_static_info_t;

typedef struct rk_aiq_sensor_nr_switch_s {
    bool valid;
    uint32_t direct;
    uint32_t up_thres;
    uint32_t down_thres;
    uint32_t div_coeff;
} rk_aiq_sensor_nr_switch_t;

typedef struct rk_aiq_sensor_dcg_ratio_s {
    bool valid;
    uint32_t integer;
    uint32_t decimal;
    uint32_t div_coeff;
} rk_aiq_sensor_dcg_ratio_t;

typedef struct {
    bool focus_support;
    bool iris_support;
    bool zoom_support;
    bool otp_valid;
    float posture;
    float hysteresis;
    float startCurrent;
    float endCurrent;

    int32_t focus_minimum;
    int32_t focus_maximum;
    int32_t zoom_minimum;
    int32_t zoom_maximum;
    int32_t zoom1_minimum;
    int32_t zoom1_maximum;
} rk_aiq_lens_descriptor;

typedef struct {
    int start_ma;
    int rated_ma;
    int step_mode;
} rk_aiq_lens_vcmcfg;

typedef struct {
    rk_aiq_af_sec_stat_t stat;
    int32_t search_num;
    int32_t pos[64];
    float sharpness[64];
} rk_aiq_af_sec_path_t;

typedef struct {
    rk_aiq_af_sec_stat_t stat;
    int32_t final_pos;
} rk_aiq_af_result_t;

typedef struct {
    int min_pos;
    int max_pos;
    float min_fl;
    float max_fl;
} rk_aiq_af_zoomrange;

typedef struct {
    int min_pos;
    int max_pos;
} rk_aiq_af_focusrange;

// sensor
typedef struct {
    unsigned short line_periods_vertical_blanking;
    unsigned short fine_integration_time_min;
    unsigned short fine_integration_time_max_margin;
    unsigned short coarse_integration_time_min;
    unsigned short coarse_integration_time_max_margin;
    unsigned int frame_length_lines;
    unsigned int line_length_pck;
    unsigned int vt_pix_clk_freq_hz;
    float pixel_clock_freq_mhz;
    unsigned short pixel_periods_per_line;
    unsigned short line_periods_per_field;
    uint32_t sensor_output_width;
    uint32_t sensor_output_height;
    uint32_t sensor_pixelformat;
    uint32_t isp_acq_width;
    uint32_t isp_acq_height;
    rk_aiq_sensor_nr_switch_t nr_switch;
    rk_aiq_sensor_dcg_ratio_t dcg_ratio;
    rk_aiq_lens_descriptor lens_des;
    struct rkmodule_awb_inf otp_awb;
    struct rkmodule_lsc_inf *otp_lsc;
    struct rkmodule_af_inf *otp_af;
    struct rkmodule_pdaf_inf *otp_pdaf;
    u8 compr_bit;
} rk_aiq_exposure_sensor_descriptor;

// exposure
typedef RkAiqExpRealParam_t rk_aiq_exposure_real_params;
typedef RkAiqExpSensorParam_t rk_aiq_exposure_sensor_params;
typedef RkAiqExpParamComb_t rk_aiq_exposure_params_comb_t;
typedef RKAiqAecExpInfo_t rk_aiq_exposure_params_t;

// focus
typedef rk_aiq_af_algo_focus_pos_t rk_aiq_focus_params_t;
// isp

#pragma pack(1)
struct rk_aiq_isp_window {
    uint16_t h_offs;
    uint16_t v_offs;
    uint16_t h_size;
    uint16_t v_size;
};
#pragma pack()

typedef RKAiqAecStats_t rk_aiq_isp_aec_stats_t;

typedef rk_aiq_af_algo_stat_v20_t rk_aiq_isp_af_stats_t;
typedef rk_aiq_af_algo_focus_pos_t rk_aiq_af_focus_pos_meas_t;

typedef rk_aiq_pdaf_algo_stat_t rk_aiq_isp_pdaf_stats_t;
typedef rk_aiq_pdaf_algo_meas_t rk_aiq_isp_pdaf_meas_t;

/**wb gain **/

#ifdef RK_SIMULATOR_HW
typedef sim_orb_stat_t rk_aiq_isp_orb_stats_t;
#else
typedef rk_aiq_orb_algo_stat_t rk_aiq_isp_orb_stats_t;
#endif
typedef rk_aiq_orb_algo_meas_t rk_aiq_isp_orb_meas_t;

typedef struct {
    unsigned char equ_segm;
    unsigned short offset;
    unsigned short gamma_y[45];
} rk_aiq_isp_goc_t;

typedef struct {
    uint32_t frame_id;
    union {
        rk_aiq_isp_aec_stats_t aec_stats; /* rv1106 ... */
        RKAiqAecStatsV25_t aec_stats_v25; /* rv1103b rk3576 */
    };
    bool bValid_aec_stats;
    int awb_hw_ver;
    union {
        rk_aiq_awb_stat_res_v200_t awb_stats_v200;
        rk_aiq_awb_stat_res2_v201_t awb_stats_v21; /* rk356x */
        rk_aiq_isp_awb_stats2_v3x_t awb_stats_v3x; /* rk3588 */
        rk_aiq_isp_awb_stats_v32_t awb_stats_v32;  /* rv1106, rk3562 */
        awbStats_stats_priv_t awb_stats_v39;       /* rv1103b rk3576 */
    };
    bool bValid_awb_stats;
    int af_hw_ver;
    union {
        rk_aiq_isp_af_stats_t af_stats;
        rk_aiq_isp_af_stats_v3x_t af_stats_v3x; /* rk3588 */
        afStats_stats_t afStats_stats;          /* rk3576 */
    };
    bool bValid_af_stats;
} rk_aiq_isp_stats_t;

#if 1
typedef AsharpProcResult_t rkaiq_asharp_procRes_t;
#endif

typedef enum rk_aiq_gray_mode_e {
    RK_AIQ_GRAY_MODE_CPSL, /*!< controlled by cpsl*/
    RK_AIQ_GRAY_MODE_OFF,
    RK_AIQ_GRAY_MODE_ON,
} rk_aiq_gray_mode_t;

/* Flash modes. Default is off.
 * Setting a flash to TORCH or INDICATOR mode will automatically
 * turn it on. Setting it to FLASH mode will not turn on the flash
 * until the FLASH_STROBE command is sent. */
enum rk_aiq_flash_mode {
    RK_AIQ_FLASH_MODE_OFF,
    RK_AIQ_FLASH_MODE_FLASH,
    RK_AIQ_FLASH_MODE_FLASH_PRE,
    RK_AIQ_FLASH_MODE_FLASH_MAIN,
    RK_AIQ_FLASH_MODE_TORCH,
    RK_AIQ_FLASH_MODE_INDICATOR,
};

/* Flash statuses, used by rk_aiq driver to check before starting
 * flash and after having started flash. */
enum rk_aiq_flash_status {
    RK_AIQ_FLASH_STATUS_OK,
    RK_AIQ_FLASH_STATUS_HW_ERROR,
    RK_AIQ_FLASH_STATUS_INTERRUPTED,
    RK_AIQ_FLASH_STATUS_TIMEOUT,
};

/* Frame status. This is used to detect corrupted frames and cpsl(compensation
 * light) exposed frames. Usually, the first 2 frames coming out of the sensor
 * are corrupted. When using flash, the frame before and the frame after
 * the cpsl exposed frame may be partially exposed by cpsl. The ISP
 * statistics for these frames should not be used by the 3A library.
 * The frame status value can be found in the "reserved" field in the
 * v4l2_buffer struct. */
enum rk_aiq_frame_status {
    RK_AIQ_FRAME_STATUS_OK,
    RK_AIQ_FRAME_STATUS_CORRUPTED,
    RK_AIQ_FRAME_STATUS_EXPOSED,
    RK_AIQ_FRAME_STATUS_PARTIAL,
    RK_AIQ_FRAME_STATUS_FAILED,
};

#define RK_AIQ_FLASH_NUM_MAX 2

typedef struct rk_aiq_flash_setting_s {
    enum rk_aiq_flash_mode flash_mode;
    enum rk_aiq_frame_status frame_status;
    float power[RK_AIQ_FLASH_NUM_MAX];
    bool strobe;
    int timeout_ms;
    int64_t effect_ts;
} rk_aiq_flash_setting_t;

typedef struct rk_aiq_ir_setting_s {
    bool irc_on; /*< true means cut the IR, vice versa */
} rk_aiq_ir_setting_t;

typedef enum rk_aiq_cpsls_e {
    RK_AIQ_CPSLS_INVALID = -1,
    RK_AIQ_CPSLS_LED = 1,
    RK_AIQ_CPSLS_IR = 2,
    RK_AIQ_CPSLS_MIX = 3, /*< led and ir mixture */
    RK_AIQ_CPSLS_MAX
} rk_aiq_cpsls_t;

/*!
 * \brief compensation light configs
 *
 * user data types of compensation lights, applied to IR and
 * full colour light source.
 */

typedef struct rk_aiq_cpsl_cfg_s {
    RKAiqOPMode_t mode;
    rk_aiq_cpsls_t lght_src;
    bool gray_on; /*!< force to gray if light on */
    union {
        struct {
            float sensitivity;    /*!< Range [0-100] */
            uint32_t sw_interval; /*!< switch interval time, unit seconds */
        } a;                      /*< auto mode */
        struct {
            uint8_t on;         /*!< disable 0, enable 1 */
            float strength_led; /*!< Range [0-100] */
            float strength_ir;  /*!< Range [0-100] */
        } m;                    /*!< manual mode */
    } u;
} rk_aiq_cpsl_cfg_t;

typedef struct rk_aiq_cpsl_info_s {
    int32_t mode;
    uint8_t on;
    bool gray;
    float strength_led;
    float strength_ir;
    float sensitivity;
    uint32_t sw_interval;
    int32_t lght_src;
} rk_aiq_cpsl_info_t;

typedef struct rk_aiq_cpsl_cap_s {
    int32_t supported_modes[RK_AIQ_OP_MODE_MAX];
    uint8_t modes_num;
    int32_t supported_lght_src[RK_AIQ_CPSLS_MAX];
    uint8_t lght_src_num;
    rk_aiq_range_t strength_led;
    rk_aiq_range_t sensitivity;
    rk_aiq_range_t strength_ir;
} rk_aiq_cpsl_cap_t;

typedef struct rk_aiq_awb_stat_res_v200_s rk_aiq_isp_raw_awb_meas_v20_t;

typedef enum capture_raw_e {
    CAPTURE_RAW_ASYNC,
    CAPTURE_RAW_SYNC,
    CAPTURE_RAW_AND_YUV_SYNC,
} capture_raw_t;

typedef enum rk_aiq_rawbuf_type_s {
    RK_AIQ_RAW_ADDR,
    RK_AIQ_RAW_FD,
    RK_AIQ_RAW_DATA,
    RK_AIQ_RAW_FILE
} rk_aiq_rawbuf_type_t;

typedef struct rk_aiq_raw_prop_s {
    uint32_t frame_width;
    uint32_t frame_height;
    rk_aiq_format_t format;
    rk_aiq_rawbuf_type_t rawbuf_type;
} rk_aiq_raw_prop_t;

typedef struct {
    uint32_t frame_id;
    uint16_t hdrProcessCnt;
    unsigned int luma[3][16];
} rk_aiq_luma_params_t;

typedef enum rk_isp_stream_mode_e {
    RK_ISP_STREAM_MODE_INVALID = -1,
    RK_ISP_STREAM_MODE_ONLNIE,
    RK_ISP_STREAM_MODE_OFFLNIE,
} rk_isp_stream_mode_t;

typedef struct {
    struct rkmodule_awb_inf otp_awb;
} rk_aiq_user_otp_info_t;

typedef enum {
    RK_ISP_RKRAWSTREAM_MODE_INVALID = 0,
    RK_ISP_RKRAWSTREAM_MODE_HALF_ONLINE,
    RK_ISP_RKRAWSTREAM_MODE_OFFLINE,
} rk_aiq_rkrawstream_mode_t;

typedef struct {
    int width;
    int height;
    rk_aiq_format_t format;
    rk_aiq_rkrawstream_mode_t mode;
} rk_aiq_rkrawstream_info_t;

typedef struct {
    char wr_mode;
    char rd_mode;
    uint16_t wr_linecnt;
    uint16_t rd_linecnt;
} __attribute__((packed)) rk_aiq_aiisp_cfg_t;

typedef struct rkisp_bay3dbuf_info_s {
    int iir_fd;
    int iir_size;
    union {
        struct {
            int cur_fd;
            int cur_size;
            int ds_fd;
            int ds_size;
        } v30;
        struct {
            int ds_fd;
            int ds_size;
        } v32;
        struct {
            int ds_fd;
            int ds_size;
            int gain_fd;
            int gain_size;
        } v33;
        struct {
            int gain_fd;
            int gain_size;
            int aiisp_fd;
            int aiisp_size;
        } v39;
    } u;
} __attribute__((packed)) rkisp_bay3dbuf_info_t;

#define RK_AIQ_CAM_GROUP_MAX_CAMS (8)

#endif
