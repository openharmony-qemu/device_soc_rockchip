#ifndef RK_AIQ_CALIB_DB_ISP20_TONECOLOR_H
#define RK_AIQ_CALIB_DB_ISP20_TONECOLOR_H

#ifndef _RK_AIQ_CALIB_TYPES_ISP20_H_
#error "Include RkAiqCalibDbTypesIsp20.h first"
#endif

typedef struct GlobalLuma_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float GlobalLumaMode;
    float envLevel[13];
    float ISO[13];
    float Tolerance;
    float globalLuma[13];
} GlobalLuma_t;

typedef struct DetailsHighLight_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float DetailsHighLightMode;
    float OEPdf[13];
    float EnvLv[13];
    float Tolerance;
    float detailsHighLight[13];
} DetailsHighLight_t;

typedef struct DetailsLowLight_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float DetailsLowLightMode;
    float FocusLuma[13];
    float DarkPdf[13];
    float ISO[13];
    float Tolerance;
    float detailsLowLight[13];
} DetailsLowLight_t;

typedef struct LocalTMO_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float LocalTMOMode;
    float DynamicRange[13];
    float EnvLv[13];
    float Tolerance;
    float Strength[13];
} LocalTMO_t;

typedef struct GlobaTMO_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float en;
    float iir;
    float mode;
    float DynamicRange[13];
    float EnvLv[13];
    float Tolerance;
    float Strength[13];
} GlobaTMO_t;

typedef struct TMO_en_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float en;
} TMO_en_t;

typedef struct CalibDb_Amerge_Para_s {
    float envLevel[13];
    float oeCurve_smooth[13];
    float oeCurve_offset[13];
    float moveCoef[13];
    float mdCurveLm_smooth[13];
    float mdCurveLm_offset[13];
    float mdCurveMs_smooth[13];
    float mdCurveMs_offset[13];
    float oeCurve_damp;
    float mdCurveLm_damp;
    float mdCurveMs_damp;
} CalibDb_Amerge_Para_t;

typedef struct CalibDb_Atmo_Para_s {
    TMO_en_t en[CALIBDB_MAX_MODE_NUM];
    GlobalLuma_t luma[CALIBDB_MAX_MODE_NUM];
    DetailsHighLight_t HighLight[CALIBDB_MAX_MODE_NUM];
    DetailsLowLight_t LowLight[CALIBDB_MAX_MODE_NUM];
    LocalTMO_t LocalTMO[CALIBDB_MAX_MODE_NUM];
    GlobaTMO_t GlobaTMO[CALIBDB_MAX_MODE_NUM];
    float damp;
} CalibDb_Atmo_Para_t;

typedef struct CalibDb_Blc_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float iso[CALIBDB_BLC_MAX_ISO_LEVEL];
    float level[4][CALIBDB_BLC_MAX_ISO_LEVEL];
} CalibDb_Blc_ModeCell_t;

typedef struct CalibDb_Blc_s {
    int enable;
    CalibDb_Blc_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_Blc_t;

typedef struct CalibDb_Dpcc_set_RK_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_sw_mindis[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_sw_mindis[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char sw_dis_scale_min[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char sw_dis_scale_max[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_RK_t;

typedef struct CalibDb_Dpcc_set_LC_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_line_thr[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_line_thr[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_line_mad_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_line_mad_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_LC_t;

typedef struct CalibDb_Dpcc_set_PG_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_pg_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_pg_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_PG_t;

typedef struct CalibDb_Dpcc_set_RND_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_rnd_thr[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_rnd_thr[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_rnd_offs[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_rnd_offs[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_RND_t;

typedef struct CalibDb_Dpcc_set_RG_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_rg_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_rg_fac[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_RG_t;

typedef struct CalibDb_Dpcc_set_RO_s {
    unsigned char rb_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char rb_ro_lim[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char g_ro_lim[CALIBDB_DPCC_MAX_ISO_LEVEL];
} CalibDb_Dpcc_set_RO_t;

typedef struct CalibDb_Dpcc_set_s {
    CalibDb_Dpcc_set_RK_t rk;
    CalibDb_Dpcc_set_LC_t lc;
    CalibDb_Dpcc_set_PG_t pg;
    CalibDb_Dpcc_set_RND_t rnd;
    CalibDb_Dpcc_set_RG_t rg;
    CalibDb_Dpcc_set_RO_t ro;
} CalibDb_Dpcc_set_t;

typedef struct CalibDb_Dpcc_Pdaf_s {
    unsigned char en;
    unsigned char point_en[16];
    unsigned short int offsetx;
    unsigned short int offsety;
    unsigned char wrapx;
    unsigned char wrapy;
    unsigned short int wrapx_num;
    unsigned short int wrapy_num;
    unsigned char point_x[16];
    unsigned char point_y[16];
    unsigned char forward_med;
} CalibDb_Dpcc_Pdaf_t;

typedef struct CalibDb_Dpcc_Expert_Mode_s {
    float iso[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_Enable[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char grayscale_mode;
    unsigned char rk_out_sel[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char dpcc_out_sel[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_rb_3x3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_g_3x3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_inc_rb_center[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_inc_g_center[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_fix_set[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set3[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set2[CALIBDB_DPCC_MAX_ISO_LEVEL];
    unsigned char stage1_use_set1[CALIBDB_DPCC_MAX_ISO_LEVEL];
    CalibDb_Dpcc_set_t set[3];
} CalibDb_Dpcc_Expert_Mode_t;

typedef struct CalibDb_Dpcc_s {
    int enable;
    char version[64];
    CalibDb_Dpcc_Fast_Mode_t fast;
    CalibDb_Dpcc_Expert_Mode_t expert;
    CalibDb_Dpcc_Pdaf_t pdaf;
    CalibDb_Dpcc_Sensor_t sensor_dpcc;
} CalibDb_Dpcc_t;

#define CALIBDB_NR_SHARP_NAME_LENGTH 64
#define CALIBDB_NR_SHARP_MODE_LENGTH 64

typedef struct CalibDb_BayerNR_Params_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    float iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float filtPara[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float luLevel[8];
    float luLevelVal[8];
    float luRatio[8][CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float fixW[4][CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float lamda;
    unsigned char gauss_en;
    float RGainOff;
    float RGainFilp;
    float BGainOff;
    float BGainFilp;
    float edgeSoftness;
    float gaussWeight0;
    float gaussWeight1;
    float bilEdgeFilter;
    float bilFilterStreng[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
    float bilEdgeSoft;
    float bilEdgeSoftRatio;
    float bilRegWgt;
} CalibDb_BayerNR_Params_t;

typedef struct CalibDb_BayerNr_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    CalibDb_BayerNR_Params_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
} CalibDb_BayerNr_ModeCell_t;

typedef struct CalibDb_BayerNr_s {
    int enable;
    char version[64];
    CalibDb_BayerNr_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_BayerNr_t;

typedef struct CalibDb_BayerNr_2_s {
    int enable;
    char version[64];
    CalibDb_BayerNr_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_BayerNr_2_t;

#define CIFISP_LSC_DATA_TBL_SIZE_ISP20 289
#define CIFISP_LSC_GRAD_TBL_SIZE_ISP20 8
#define CIFISP_LSC_SIZE_TBL_SIZE_ISP20 8
#define LSC_GRAD_TBL_SIZE_ISP20 8
#define LSC_ILLUMINATION_MAX 10
typedef enum CalibDb_Used_For_Case_e {
    USED_FOR_CASE_NORMAL = 0,
    USED_FOR_CASE_FLASH,
    USED_FOR_CASE_GRAY,
    USED_FOR_CASE_3,
    USED_FOR_CASE_MAX
} CalibDb_Used_For_Case_t;

#define LSC_RESOLUTION_NAME (15U)
typedef char CalibDb_ResolutionName_t[LSC_RESOLUTION_NAME];
#define LSC_PROFILE_NAME (25U)
typedef char CalibDb_Lsc_ProfileName_t[LSC_PROFILE_NAME];
#define LSC_ILLUMINATION_NAME (20U)
typedef char CalibDb_IlluminationName_t[LSC_ILLUMINATION_NAME];
#ifndef LSC_PROFILES_NUM_MAX
#define LSC_PROFILES_NUM_MAX (5)
#endif
#ifndef LSC_RESOLUTIONS_NUM_MAX
#define LSC_RESOLUTIONS_NUM_MAX (4)
#endif
typedef enum Cam4ChColorComponent_e {
    CAM_4CH_COLOR_COMPONENT_RED = 0,
    CAM_4CH_COLOR_COMPONENT_GREENR = 1,
    CAM_4CH_COLOR_COMPONENT_GREENB = 2,
    CAM_4CH_COLOR_COMPONENT_BLUE = 3,
    CAM_4CH_COLOR_COMPONENT_MAX
} Cam4ChColorComponent_t;

typedef struct CamLscMatrix_s {
    Cam17x17UShortMatrix_t LscMatrix[CAM_4CH_COLOR_COMPONENT_MAX];
} CamLscMatrix_t;

typedef struct CamVignettingCurve_s {
    uint16_t arraySize;
    float pSensorGain[4];  //change to pointer
    float pVignetting[4];
} CamVignettingCurve_t;

typedef struct CalibDb_AlscCof_ill_s {
    char illuName[25];
    float awbGain[2];
    int tableUsedNO;
    CalibDb_Lsc_ProfileName_t tableUsed[LSC_PROFILES_NUM_MAX];
    CamVignettingCurve_t vignettingCurve;
} CalibDb_AlscCof_ill_t;

typedef struct CalibDb_AlscCof_s {
    int lscResNum;
    CalibDb_ResolutionName_t lscResName[LSC_RESOLUTIONS_NUM_MAX];  // type CalibDb_ResolutionName_t
    int illuNum[USED_FOR_CASE_MAX];
    CalibDb_AlscCof_ill_t illAll[USED_FOR_CASE_MAX][LSC_ILLUMINATION_MAX];
    int usedForCaseAll[USED_FOR_CASE_MAX * LSC_ILLUMINATION_MAX];  //for write xml
} CalibDb_AlscCof_t;

typedef struct CalibDb_LscTableProfile_s {
    CalibDb_Lsc_ProfileName_t name;          /**< profile name */
    CalibDb_ResolutionName_t resolution;     /**< resolution link */
    CalibDb_IlluminationName_t illumination; /**< illumination link */
    float vignetting;                        /**< vignetting value */
    uint16_t LscSectors;
    uint16_t LscNo;
    uint16_t LscXo;
    uint16_t LscYo;
    uint16_t LscXGradTbl[LSC_GRAD_TBL_SIZE_ISP20];
    uint16_t LscYGradTbl[LSC_GRAD_TBL_SIZE_ISP20];
    uint16_t LscXSizeTbl[CIFISP_LSC_SIZE_TBL_SIZE_ISP20];
    uint16_t LscYSizeTbl[CIFISP_LSC_SIZE_TBL_SIZE_ISP20];
    Cam17x17UShortMatrix_t LscMatrix[CAM_4CH_COLOR_COMPONENT_MAX]; /**< matrix for different color channels */
} CalibDb_LscTableProfile_t;

typedef struct CalibDb_Lsc_s {
    bool enable;
    bool damp_enable;
    CalibDb_AlscCof_t aLscCof;
    int tableAllNum;
    CalibDb_LscTableProfile_t *tableAll;  //type  CalibDb_LscTableProfile_t;
} CalibDb_Lsc_t;

typedef struct CalibDb_RKDM_s {
    unsigned char debayer_en;
    signed char debayer_filter1[5];
    signed char debayer_filter2[5];
    unsigned char debayer_gain_offset;
    int ISO[9];
    unsigned char sharp_strength[9];
    unsigned short debayer_hf_offset[9];
    unsigned char debayer_offset;
    unsigned char debayer_clip_en;
    unsigned char debayer_filter_g_en;
    unsigned char debayer_filter_c_en;
    unsigned char debayer_thed0;
    unsigned char debayer_thed1;
    unsigned char debayer_dist_scale;
    unsigned char debayer_cnr_strength;
    unsigned char debayer_shift_num;
} CalibDb_RKDM_t;

#define CCM_ILLUMINATION_MAX 9
#define CCM_PROFILE_NAME (25U)
typedef char CalibDb_Ccm_ProfileName_t[CCM_PROFILE_NAME];
#define CCM_ILLUMINATION_NAME (20U)
#define CCM_PROFILES_NUM_MAX (5)
#define CCM_RESOLUTIONS_NUM_MAX (4)
#define CALIBDB_ISO_NUM (9)
typedef enum Cam3ChColorComponent_e {
    CAM_3CH_COLOR_COMPONENT_RED = 0,
    CAM_3CH_COLOR_COMPONENT_GREEN = 1,
    CAM_3CH_COLOR_COMPONENT_BLUE = 2,
    CAM_3CH_COLOR_COMPONENT_MAX
} Cam3ChColorComponent_t;

typedef struct CamSaturationCurve_s {
    uint16_t arraySize;
    float pSensorGain[4];  //change to pointer
    float pSaturation[4];
} CamSaturationCurve_t;

typedef struct CalibDb_AccmCof_ill_s {
    char illuName[25];
    float awbGain[2];
    int matrixUsedNO;
    CalibDb_Ccm_ProfileName_t matrixUsed[CCM_PROFILES_NUM_MAX];
    CamSaturationCurve_t saturationCurve;
} CalibDb_AccmCof_ill_t;

typedef struct CalibDb_AccmCof_s {
    int illuNum;
    CalibDb_AccmCof_ill_t illAll[CCM_ILLUMINATION_MAX];
} CalibDb_AccmCof_t;

typedef struct CalibDb_CcmMatrixProfile_s {
    CalibDb_Ccm_ProfileName_t name; /**< profile name */
    CalibDb_IlluminationName_t illumination;
    float saturation;                    /**< saturation value */
    Cam3x3FloatMatrix_t CrossTalkCoeff;  /**< CrossTalk matrix coefficients */
    Cam1x3FloatMatrix_t CrossTalkOffset; /**< CrossTalk offsets */
} CalibDb_CcmMatrixProfile_t;

typedef struct CalibDb_LUMA_CCM_s {
    float rgb2y_para[3];
    float low_bound_pos_bit;
    float y_alpha_curve[17];
    int gain_scale_cure_size;
    float alpha_gain[9];  //change to pointer
    float alpha_scale[9];
} CalibDb_LUMA_CCM_t;

typedef struct CalibDb_Ccm_ModeCell_s {
    bool valid;
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    bool damp_enable;
    CalibDb_AccmCof_t aCcmCof;
    CalibDb_LUMA_CCM_t luma_ccm;
    int matrixAllNum;
    CalibDb_CcmMatrixProfile_t matrixAll[CCM_RESOLUTIONS_NUM_MAX * CCM_ILLUMINATION_MAX *
                                         CCM_PROFILES_NUM_MAX];  //type  CalibDb_CcmMatrixProfile_t;
} CalibDb_Ccm_ModeCell_t;

typedef enum CalibDb_CcmHdrNormalMode_e {
    CCM_FOR_MODE_NORMAL = 0,
    CCM_FOR_MODE_HDR,
    CCM_FOR_MODE_MAX,
} CalibDb_CcmHdrNormalMode_t;

typedef struct CalibDb_Ccm_s {
    int modecellNum;
    bool enable;
    CalibDb_Ccm_ModeCell_t mode_cell[CCM_FOR_MODE_MAX];
} CalibDb_Ccm_t;

#endif
