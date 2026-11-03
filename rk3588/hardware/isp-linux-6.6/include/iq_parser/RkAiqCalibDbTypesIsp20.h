#ifndef _RK_AIQ_CALIB_TYPES_ISP20_H_
#define _RK_AIQ_CALIB_TYPES_ISP20_H_

#include "algos/adpcc/rk_aiq_types_adpcc_ext.h"

#pragma pack(4)

#define CALIBDB_MAX_ISO_LEVEL 13
#define CALIBDB_NR_SHARP_MAX_ISO_LEVEL CALIBDB_MAX_ISO_LEVEL
#define CALIBDB_DPCC_MAX_ISO_LEVEL CALIBDB_MAX_ISO_LEVEL
#define CALIBDB_BLC_MAX_ISO_LEVEL CALIBDB_MAX_ISO_LEVEL
#define CALIBDB_NR_SHARP_SETTING_LEVEL 6
#define CALIBDB_MAX_MODE_NUM 5
#define CALIBDB_MAX_MODE_NAME_LENGTH (20)
#define CALIBDB_ZOOM_FOCUS_TBL_SIZE 256
#define CALIBDB_DEGAMMA_CRUVE_KNOTS 17

#define CALIBDB_MAX_SCENE_NAME_LENGTH (10)
#include "RkAiqCalibDbIsp20Aec.h"
#include "RkAiqCalibDbIsp20SensorAwb.h"
#include "RkAiqCalibDbIsp20ToneColor.h"
#include "RkAiqCalibDbIsp20NoiseGamma.h"
struct CalibDb_Sharp_ISO_s {
    float iso;
    float hratio;
    float lratio;
    float mf_sharp_ratio;
    float hf_sharp_ratio;
    float luma_sigma[8];
    float pbf_gain;
    float pbf_ratio;
    float pbf_add;
    float mf_clip_pos[8];
    float mf_clip_neg[8];
    float hf_clip[8];
    float mbf_gain;
    float hbf_gain;
    float hbf_ratio;
    float mbf_add;
    float hbf_add;
    float local_sharp_strength;
    float pbf_coeff_percent;
    float rf_m_coeff_percent;
    float rf_h_coeff_percent;
    float hbf_coeff_percent;
};

typedef struct CalibDb_Sharp_Setting_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    struct CalibDb_Sharp_ISO_s sharp_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_Sharp_Setting_t;

typedef struct CalibDb_Sharp_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float gauss_luma_coeff[9];
    float mbf_coeff[221];
    //v2
    float pbf_coeff_l[9];
    float pbf_coeff_h[9];
    float rf_m_coeff_l[25];
    float rf_m_coeff_h[25];
    float rf_h_coeff_l[25];
    float rf_h_coeff_h[25];
    float hbf_coeff_l[9];
    float hbf_coeff_h[9];
    CalibDb_Sharp_Setting_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
} CalibDb_Sharp_ModeCell_t;

typedef struct CalibDb_Sharp_s {
    int enable;
    char version[64];
    float luma_point[8];
    CalibDb_Sharp_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_Sharp_t;

typedef struct CalibDb_Sharp_2_s {
    int enable;
    char version[64];
    float luma_point[8];
    CalibDb_Sharp_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_Sharp_2_t;

struct CalibDb_EdgeFilter_ISO_s {
    float iso;
    float edge_thed;
    float src_wgt;
    unsigned char alpha_adp_en;
    float local_alpha;
    float global_alpha;
    float noise_clip[8];
    float dog_clip_pos[8];
    float dog_clip_neg[8];
    float dog_alpha[8];
    float direct_filter_coeff[5];
    float dog_kernel_row0[5];
    float dog_kernel_row1[5];
    float dog_kernel_row2[5];
    float dog_kernel_row3[5];
    float dog_kernel_row4[5];
    float dog_kernel_percent;
};

typedef struct CalibDb_EdgeFilter_Setting_s {
    char snr_mode[CALIBDB_NR_SHARP_NAME_LENGTH];
    char sensor_mode[CALIBDB_NR_SHARP_MODE_LENGTH];
    struct CalibDb_EdgeFilter_ISO_s edgeFilter_iso[CALIBDB_NR_SHARP_MAX_ISO_LEVEL];
} CalibDb_EdgeFilter_Setting_t;

typedef struct CalibDb_EdgeFilter_ModeCell_s {
    char name[CALIBDB_MAX_MODE_NAME_LENGTH];
    float dog_kernel_l[25];
    float dog_kernel_h[25];
    CalibDb_EdgeFilter_Setting_t setting[CALIBDB_NR_SHARP_SETTING_LEVEL];
} CalibDb_EdgeFilter_ModeCell_t;

typedef struct CalibDb_EdgeFilter_s {
    int enable;
    char version[64];
    float luma_point[8];
    CalibDb_EdgeFilter_ModeCell_t mode_cell[CALIBDB_MAX_MODE_NUM];
} CalibDb_EdgeFilter_t;

typedef struct CalibDb_EdgeFilter_2_s {
    int enable;
    char version[64];
    float luma_point[8];
    CalibDb_EdgeFilter_ModeCell_t *mode_cell;
    int mode_num;
} CalibDb_EdgeFilter_2_t;

typedef struct CalibDb_IIR_setting_v20_s {
    float stab_fnum;
    float sigma;
    float wt_sigma;
    float air_sigma;
    float tmax_sigma;
} CalibDb_IIR_setting_v20_t;

typedef struct CalibDb_Dehaze_Setting_v20_s {
    float en;
    float iso[9];
    float dc_min_th[9];
    float dc_max_th[9];
    float yhist_th[9];
    float yblk_th[9];
    float dark_th[9];
    float bright_min[9];
    float bright_max[9];
    float wt_max[9];
    float air_min[9];
    float air_max[9];
    float tmax_base[9];
    float tmax_off[9];
    float tmax_max[9];
    float cfg_wt[9];
    float cfg_air[9];
    float cfg_tmax[9];
    float dc_thed[9];
    float dc_weitcur[9];
    float air_thed[9];
    float air_weitcur[9];
    CalibDb_IIR_setting_v20_t IIR_setting;
} CalibDb_Dehaze_Setting_v20_t;

typedef struct CalibDb_Enhance_Setting_v20_s {
    float en;
    float iso[9];
    float enhance_value[9];
} CalibDb_Enhance_Setting_v20_t;

typedef struct CalibDb_Hist_setting_v20_s {
    float en;
    float iso[9];
    unsigned char hist_channel[9];
    unsigned char hist_para_en[9];
    float hist_gratio[9];
    float hist_th_off[9];
    float hist_k[9];
    float hist_min[9];
    float hist_scale[9];
    float cfg_gratio[9];
} CalibDb_Hist_setting_v20_t;

typedef struct CalibDb_Dehaze_ModeCell_v20_s {
    char scene[CALIBDB_MAX_MODE_NAME_LENGTH];
    float en;
    float cfg_alpha;
    CalibDb_Dehaze_Setting_v20_t dehaze_setting;
    CalibDb_Enhance_Setting_v20_t enhance_setting;
    CalibDb_Hist_setting_v20_t hist_setting;
} CalibDb_Dehaze_ModeCell_v20_t;

typedef struct CalibDb_Dehaze_s {
    CalibDb_Dehaze_ModeCell_v20_t calib_v20[CALIBDB_MAX_MODE_NUM];
    CalibDb_Dehaze_ModeCell_v20_t tuning_v20[CALIBDB_MAX_MODE_NUM];
} CalibDb_Dehaze_t;

typedef enum CalibDb_Af_SearchDir_s {
    CAM_AFM_POSITIVE_SEARCH = 0,
    CAM_AFM_NEGATIVE_SEARCH = 1,
    CAM_AFM_ADAPTIVE_SEARCH = 2
} CalibDb_Af_SearchDir_t;

typedef enum CalibDb_Af_SS_s {
    CAM_AFM_FSS_INVALID = 0,
    CAM_AFM_FSS_FULLRANGE = 1,      /**< scan the full focus range to find the point of best focus */
    CAM_AFM_FSS_HILLCLIMBING = 2,   /**< use hillclimbing search */
    CAM_AFM_FSS_ADAPTIVE_RANGE = 3, /**< similar to full range search, but with multiple subsequent scans
                                         with decreasing range and step size will be performed. */
    CAM_AFM_FSS_MUTIWINDOW = 4,     /**< search by muti-window statistics */
    CAM_AFM_FSS_MAX
} CalibDb_Af_SS_t;

typedef struct CalibDb_Af_Contrast_s {
    unsigned char enable;
    CalibDb_Af_SS_t Afss; /**< enumeration type for search strategy */
    CalibDb_Af_SearchDir_t FullDir;
    unsigned char FullSteps;
    unsigned short FullRangeTbl[65]; /**< full range search table*/
    CalibDb_Af_SearchDir_t AdaptiveDir;
    unsigned char AdaptiveSteps;
    unsigned short AdaptRangeTbl[65]; /**< adaptive range search table*/
    float TrigThers;                  /**< AF trigger threshold */
    float LumaTrigThers;
    float StableThers;           /**< AF stable threshold */
    unsigned short StableFrames; /**< AF stable  status must hold frames */
    unsigned short StableTime;   /**< AF stable status must hold time */
    unsigned char SceneDiffEnable;
    float SceneDiffThers;
    unsigned short SceneDiffBlkThers;
    float CenterSceneDiffThers;
    float ValidMaxMinRatio;
    float ValidValueThers;
    float OutFocusValue;        /**< out of focus vlaue*/
    unsigned short OutFocusPos; /**< out of focus position*/
    unsigned char WeightEnable;
    unsigned short Weight[225]; /**< weight */
    unsigned char SearchPauseLumaEnable;
    float SearchPauseLumaThers;
    unsigned short SearchLumaStableFrames;
    float SearchLumaStableThers;
    float FlatValue;
} CalibDb_Af_Contrast_t;

typedef struct CalibDb_Af_Laser_s {
    unsigned char enable;
    float vcmDot[7];
    float distanceDot[7];
} CalibDb_Af_Laser_t;

typedef struct CalibDb_Af_Pdaf_s {
    unsigned char enable;
} CalibDb_Af_Pdaf_t;

typedef struct CalibDb_Af_VcmCfg_s {
    int start_current;
    int rated_current;
    int step_mode;
    int extra_delay;
} CalibDb_Af_VcmCfg_t;

typedef struct CalibDb_Af_MeasIsoCfg_s {
    int iso;
    unsigned short afmThres;
    unsigned short gammaY[17];
    unsigned char gaussWeight[3];
} CalibDb_Af_MeasIsoCfg_t;

typedef struct CalibDb_Af_DefCode_s {
    unsigned char code;
} CalibDb_Af_DefCode_t;

typedef struct CalibDb_Af_ZoomFocusTbl_s {
    int tbl_len;
    float focal_length[CALIBDB_ZOOM_FOCUS_TBL_SIZE];
    int zoom_pos[CALIBDB_ZOOM_FOCUS_TBL_SIZE];
    int focus_infpos[CALIBDB_ZOOM_FOCUS_TBL_SIZE];
    int focus_macropos[CALIBDB_ZOOM_FOCUS_TBL_SIZE];
} CalibDb_Af_ZoomFocusTbl_t;

typedef struct CalibDb_AF_s {
    signed char af_mode;
    unsigned short win_h_offs;
    unsigned short win_v_offs;
    unsigned short win_h_size;
    unsigned short win_v_size;
    CalibDb_Af_DefCode_t fixed_mode;
    CalibDb_Af_DefCode_t macro_mode;
    CalibDb_Af_DefCode_t infinity_mode;
    CalibDb_Af_Contrast_t contrast_af;
    CalibDb_Af_Laser_t laser_af;
    CalibDb_Af_Pdaf_t pdaf;
    CalibDb_Af_VcmCfg_t vcmcfg;
    CalibDb_Af_MeasIsoCfg_t measiso_cfg[CALIBDB_MAX_ISO_LEVEL];
    CalibDb_Af_ZoomFocusTbl_t zoomfocus_tbl;
} CalibDb_AF_t;

typedef struct CalibDb_ORB_s {
    unsigned char orb_en;
} CalibDb_ORB_t;

typedef struct CalibDb_LUMA_DETECT_s {
    unsigned char luma_detect_en;
    int fixed_times;
    float mutation_threshold;
    float mutation_threshold_level2;
} CalibDb_LUMA_DETECT_t;

typedef struct CalibDb_FEC_s {
    unsigned char fec_en;
    char meshfile[256];
    int correct_level;
    double light_center[2];  // light center
    double coefficient[4];   // the distortion coefficient of the fisheye lens
} CalibDb_FEC_t;

typedef struct CalibDb_EIS_s {
    unsigned char eis_en;
} CalibDb_EIS_t;

typedef struct CalibDb_LDCH_s {
    unsigned char ldch_en;
    char meshfile[256];
    int correct_level;
    int correct_level_max;
    double light_center[2];  // light center
    double coefficient[4];   // the distortion coefficient of the fisheye lens
} CalibDb_LDCH_t;
typedef struct {
    bool enable;
    unsigned short look_up_table_r[729];
    unsigned short look_up_table_g[729];
    unsigned short look_up_table_b[729];
} CalibDb_Lut3d_t;

typedef struct {
    bool support_en;
    RKAiqOPMode_t dcg_optype;
    Cam1x3IntMatrix_t dcg_mode;
    float dcg_ratio;
    bool gainCtrl_en;
    bool envCtrl_en;
    bool sync_switch;
    float lcg2hcg_gain_th;
    float lcg2hcg_env_th;
    float hcg2lcg_gain_th;
    float hcg2lcg_env_th;
} CalibDb_Dcg_Params_t;

typedef struct {
    CalibDb_Dcg_Params_t Hdr;
    CalibDb_Dcg_Params_t Normal;
} CalibDb_Dcg_t;

typedef struct {
    uint8_t support_en;
    /* default mode
     * 0: auto
     * 1: manual
     */
    int32_t mode;
    /* force gray if cpsl on */
    uint8_t gray;
    /* default cpsl source
     * 0: led
     * 1: ir
     * 2: mix
     */
    int32_t lght_src;
    /* auto mode default params */
    float ajust_sens;
    uint32_t sw_interval;
    uint32_t on2off_th;
    uint32_t off2on_th;
    /* manual mode default params */
    uint8_t cpsl_on;
    float strength;
} CalibDb_Cpsl_t;

typedef struct {
    int time_delay;
    int gain_delay;
    int dcg_delay;
} CalibDb_ExpDelay_t;

typedef struct {
    CalibDb_ExpDelay_t Hdr;
    CalibDb_ExpDelay_t Normal;
} CalibDb_ExpDelay_comb_t;

typedef struct {
#define HDR_MODE_2_FRAME_STR "MODE_2_FRAME"
#define HDR_MODE_2_LINE_STR "MODE_2_LINE"
#define HDR_MODE_3_FRAME_STR "MODE_3_FRAME"
#define HDR_MODE_3_LINE_STR "MODE_3_LINE"
#define HDR_LINE_MODE_DCG_STR "DCG"
#define HDR_LINE_MODE_STAGGER_STR "STAGGER"
    unsigned char hdr_en;
    rk_aiq_isp_hdr_mode_t hdr_mode;
    rk_aiq_sensor_hdr_line_mode_t line_mode;
    CalibDb_Dcg_t dcg;
    CalibDb_ExpDelay_comb_t exp_delay;
} CalibDb_System_t;

typedef struct {
    char parse_version[16];
    char date[16];
    char author[32];
    char sensor_name[32];
    char sample_name[64];
    char gen_verion[16];
    uint32_t magic_code;
} CalibDb_Header_t;

typedef struct CalibDb_ColorAsGrey_s {
    int enable;
} CalibDb_ColorAsGrey_t;

typedef struct CalibDb_cProc_s {
    uint8_t enable;
    uint8_t brightness;
    uint8_t contrast;
    uint8_t saturation;
    uint8_t hue;
} CalibDb_cProc_t;

typedef struct CalibDb_IE_s {
    int enable;
    int mode;
} CalibDb_IE_t;

typedef struct CamCalibDbContextIsp20_s {
    CalibDb_Header_t header;
    struct list_head awb_calib_para_v200;
    struct list_head awb_adjust_para;
    CalibDb_Lut3d_t lut3d;
    CalibDb_Aec_Para_t aec;
    CalibDb_AF_t af;
    CalibDb_Amerge_Para_t amerge;
    CalibDb_Atmo_Para_t atmo;
    CalibDb_Blc_t blc;
    CalibDb_Dpcc_t dpcc;
    CalibDb_BayerNr_2_t bayerNr;
    CalibDb_Lsc_t lsc;
    CalibDb_RKDM_t dm;
    CalibDb_Ccm_t ccm;
    CalibDb_UVNR_2_t uvnr;
    CalibDb_Gamma_t gamma;
    CalibDb_Degamma_t degamma;
    CalibDb_YNR_2_t ynr;
    CalibDb_Gic_t gic;
    CalibDb_MFNR_2_t mfnr;
    CalibDb_Sharp_2_t sharp;
    CalibDb_EdgeFilter_2_t edgeFilter;
    CalibDb_Dehaze_t dehaze;
    CalibDb_FEC_t afec;
    CalibDb_EIS_t aeis;
    CalibDb_LDCH_t aldch;
    CalibDb_LUMA_DETECT_t lumaDetect;
    CalibDb_ORB_t orb;
    CalibDb_Sensor_Para_t sensor;
    CalibDb_Module_Info_t module;
    CalibDb_Cpsl_t cpsl;
    CalibDb_ColorAsGrey_t colorAsGrey;
    CalibDb_cProc_t cProc;
    CalibDb_IE_t ie;
    CalibDb_System_t sysContrl;
} CamCalibDbContextIsp20_t;

#pragma pack()
#endif
