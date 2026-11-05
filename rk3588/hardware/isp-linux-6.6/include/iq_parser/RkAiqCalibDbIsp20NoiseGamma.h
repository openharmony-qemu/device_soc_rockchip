#ifndef RK_AIQ_CALIB_DB_ISP20_NOISEGAMMA_H
#define RK_AIQ_CALIB_DB_ISP20_NOISEGAMMA_H

#ifndef _RK_AIQ_CALIB_TYPES_ISP20_H_
#error "Include RkAiqCalibDbTypesIsp20.h first"
#endif

typedef struct CalibDb_UVNR_Params_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    float ISO[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step0_uvgrad_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step0_uvgrad_offset[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_nonMed1[4];
    float step1_nonBf1[4];
    float step1_downSample_w[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_downSample_h[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_downSample_meansize[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_median_IIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step1_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_nonExt_block[4];
    float step2_nonMed[4];
    float step2_nonBf[4];
    float step2_downSample_w[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_downSample_h[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_downSample_meansize[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_median_IIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step2_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_nonBf3[4];
    float step3_bf_sigmaR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_uvgain[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_ratio[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_size[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_sigmaD[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_isRowIIR[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float step3_bf_isYcopy[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float kernel_3x3[3];
    float kernel_5x5[5];
    float kernel_9x9[8];
    float kernel_9x9_num;
    float sigma_adj_luma[9];
    float sigma_adj_ratio[9];
    float threshold_adj_luma[9];
    float threshold_adj_thre[9];
} CalibDb_UVNR_Params_t;

typedef struct CalibDb_UVNR_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    CalibDb_UVNR_Params_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
} CalibDb_UVNR_ModeCell_t;

typedef struct CalibDb_UVNR_s {
    int enable;
    char version[64];
    CalibDb_UVNR_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_UVNR_t;

typedef struct CalibDb_UVNR_2_s {
    int enable;
    char version[64];
    CalibDb_UVNR_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_UVNR_2_t;

typedef struct CalibDb_Gamma_s {
    unsigned char gamma_en;
    unsigned char gamma_out_segnum;
    unsigned char gamma_out_offset;
    float curve_normal[45];
    float curve_hdr[45];
    float curve_night[45];
} CalibDb_Gamma_t;

typedef struct CalibDb_Degamma_para_s {
    unsigned char degamma_scene_en;
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float X_axis[CALIBDB_DEGAMMA_CRUVE_KNOTS];
    float curve_R[CALIBDB_DEGAMMA_CRUVE_KNOTS];
    float curve_G[CALIBDB_DEGAMMA_CRUVE_KNOTS];
    float curve_B[CALIBDB_DEGAMMA_CRUVE_KNOTS];
} CalibDb_Degamma_para_t;

typedef struct CalibDb_Degamma_s {
    unsigned char degamma_en;
    CalibDb_Degamma_para_t mode[3];
} CalibDb_Degamma_t;

typedef struct CalibDb_YNR_ISO_s {
    float iso;
    double sigma_curve[5];
    float ynr_lci[4];
    float ynr_lhci[4];
    float ynr_hlci[4];
    float ynr_hhci[4];
    float lo_lumaPoint[6];
    float lo_lumaRatio[6];
    float lo_directionStrength;
    float lo_bfScale[4];
    float imerge_ratio;
    float imerge_bound;
    float denoise_weight[4];
    float hi_lumaPoint[6];
    float hi_lumaRatio[6];
    float hi_bfScale[4];
    float hwith_d[4];
    float hi_denoiseStrength;
    float hi_detailMinAdjDnW;
    float hi_denoiseWeight[4];
    float y_luma_point[6];
    float hgrad_y_level1[6];
    float hgrad_y_level2[6];
    float hgrad_y_level3[6];
    float hgrad_y_level4[6];
    float hi_soft_thresh_scale[4];
} CalibDb_YNR_ISO_t;

typedef struct CalibDb_YNR_Setting_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    CalibDb_YNR_ISO_t ynr_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_YNR_Setting_t;

typedef struct CalibDb_YNR_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    CalibDb_YNR_Setting_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
} CalibDb_YNR_ModeCell_t;

typedef struct CalibDb_YNR_s {
    int enable;
    char version[64];
    CalibDb_YNR_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_YNR_t;

typedef struct CalibDb_YNR_2_s {
    int enable;
    char version[64];
    CalibDb_YNR_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_YNR_2_t;

typedef struct CalibDb_Gic_setting_v20_s {
    float iso[CALIBDB_ISO_NUM];
    float min_busy_thre[CALIBDB_ISO_NUM];
    float min_grad_thr1[CALIBDB_ISO_NUM];
    float min_grad_thr2[CALIBDB_ISO_NUM];
    float k_grad1[CALIBDB_ISO_NUM];
    float k_grad2[CALIBDB_ISO_NUM];
    float gb_thre[CALIBDB_ISO_NUM];
    float maxCorV[CALIBDB_ISO_NUM];
    float maxCorVboth[CALIBDB_ISO_NUM];
    float dark_thre[CALIBDB_ISO_NUM];
    float dark_threHi[CALIBDB_ISO_NUM];
    float k_grad1_dark[CALIBDB_ISO_NUM];
    float k_grad2_dark[CALIBDB_ISO_NUM];
    float min_grad_thr_dark1[CALIBDB_ISO_NUM];
    float min_grad_thr_dark2[CALIBDB_ISO_NUM];
    float noiseCurve_0[CALIBDB_ISO_NUM];
    float noiseCurve_1[CALIBDB_ISO_NUM];
    float GValueLimitLo[CALIBDB_ISO_NUM];
    float GValueLimitHi[CALIBDB_ISO_NUM];
    float textureStrength[CALIBDB_ISO_NUM];
    float ScaleLo[CALIBDB_ISO_NUM];
    float ScaleHi[CALIBDB_ISO_NUM];
    float globalStrength[CALIBDB_ISO_NUM];
    float noise_coea[CALIBDB_ISO_NUM];
    float noise_coeb[CALIBDB_ISO_NUM];
    float diff_clip[CALIBDB_ISO_NUM];
} CalibDb_Gic_setting_v20_t;

typedef struct CalibDb_Gic_ModeCell_v20_s {
    char scene[CALIBDB_MAX_MODE_NAME_LENGTH];
    unsigned char gic_en;
    unsigned char edge_en;
    unsigned char gr_ration;
    unsigned char noise_cut_en;
    CalibDb_Gic_setting_v20_t setting;
} CalibDb_Gic_ModeCell_v20_t;

typedef struct CalibDb_Gic_s {
    CalibDb_Gic_ModeCell_v20_t calib_v20[CALIBDB_MAX_MODE_NUM];
    CalibDb_Gic_ModeCell_v20_t tuning_v20[CALIBDB_MAX_MODE_NUM];
} CalibDb_Gic_t;

struct CalibDb_awb_uv_ratio_s {
    char illum[9];
    float ratio[2];
};

struct CalibDb_MFNR_ISO_s {
    float iso;
    float weight_limit_y[4];
    float weight_limit_uv[3];
    float ratio_frq[4];
    float luma_w_in_chroma[3];
    double noise_curve[5];
    double noise_curve_x00;
    float y_lo_noiseprofile[4];
    float y_hi_noiseprofile[4];
    float y_lo_denoiseweight[4];
    float y_hi_denoiseweight[4];
    float y_lo_bfscale[4];
    float y_hi_bfscale[4];
    float y_lumanrpoint[6];
    float y_lumanrcurve[6];
    float y_denoisestrength;
    float y_lo_lvl0_gfdelta[6];
    float y_hi_lvl0_gfdelta[6];
    float y_lo_lvl1_gfdelta[3];
    float y_hi_lvl1_gfdelta[3];
    float y_lo_lvl2_gfdelta[3];
    float y_hi_lvl2_gfdelta[3];
    float y_lo_lvl3_gfdelta[3];
    float y_hi_lvl3_gfdelta[3];
    float uv_lo_noiseprofile[3];
    float uv_hi_noiseprofile[3];
    float uv_lo_denoiseweight[3];
    float uv_hi_denoiseweight[3];
    float uv_lo_bfscale[3];
    float uv_hi_bfscale[3];
    float uv_lumanrpoint[6];
    float uv_lumanrcurve[6];
    float uv_denoisestrength;
    float uv_lo_lvl0_gfdelta[6];
    float uv_hi_lvl0_gfdelta[6];
    float uv_lo_lvl1_gfdelta[3];
    float uv_hi_lvl1_gfdelta[3];
    float uv_lo_lvl2_gfdelta[3];
    float uv_hi_lvl2_gfdelta[3];
    float lvl0_gfsigma[6];
    float lvl1_gfsigma[3];
    float lvl2_gfsigma[3];
    float lvl3_gfsigma[3];
};

typedef struct CalibDb_MFNR_Setting_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    struct CalibDb_MFNR_ISO_s mfnr_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_MFNR_Setting_t;

typedef struct CalibDb_MFNR_Dynamic_s {
    int enable;
    float lowth_iso;
    float lowth_time;
    float highth_iso;
    float highth_time;
} CalibDb_MFNR_Dynamic_t;

typedef struct CalibDb_MFNR_Motion_s {
    int enable;
    float iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float sigmaHScale[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float sigmaLScale[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float lightClp[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float uvWeight[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float mfnrSigmaScale[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float yuvnrGainScale0[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float yuvnrGainScale1[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float yuvnrGainScale2[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved0[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved1[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved2[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved3[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved4[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved5[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved6[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float reserved7[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float frame_limit_uv[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float frame_limit_y[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_MFNR_Motion_t;

typedef struct CalibDb_MFNR_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    CalibDb_MFNR_Setting_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
    CalibDb_MFNR_Dynamic_t dynamic;
    CalibDb_MFNR_Motion_t motion;
} CalibDb_MFNR_ModeCell_t;

typedef struct CalibDb_MFNR_s {
    int enable;
    char version[64];
    unsigned char local_gain_en;
    unsigned char motion_detect_en;
    unsigned char mode_3to1;
    unsigned char max_level;
    unsigned char max_level_uv;
    unsigned char back_ref_num;
    struct CalibDb_awb_uv_ratio_s uv_ratio[4];
    CalibDb_MFNR_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_MFNR_t;

typedef struct CalibDb_MFNR_2_s {
    int enable;
    char version[64];
    unsigned char local_gain_en;
    unsigned char motion_detect_en;
    unsigned char mode_3to1;
    unsigned char max_level;
    unsigned char max_level_uv;
    unsigned char back_ref_num;
    struct CalibDb_awb_uv_ratio_s uv_ratio[4];
    CalibDb_MFNR_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_MFNR_2_t;

#endif
