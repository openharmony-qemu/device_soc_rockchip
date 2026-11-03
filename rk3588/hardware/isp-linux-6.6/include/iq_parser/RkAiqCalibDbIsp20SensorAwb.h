#ifndef RK_AIQ_CALIB_DB_ISP20_SENSORAWB_H
#define RK_AIQ_CALIB_DB_ISP20_SENSORAWB_H

#ifndef _RK_AIQ_CALIB_TYPES_ISP20_H_
#error "Include RkAiqCalibDbTypesIsp20.h first"
#endif

#define CALD_AEC_GAIN_RANGE_MAX_LEN 350
typedef enum _CalibDb_ExpGainMode_e {
    RKAIQ_EXPGAIN_MODE_NONLINEAR_DB = 0,
    RKAIQ_EXPGAIN_MODE_LINEAR = 1,
    RKAIQ_EXPGAIN_MODE_MAX = 2
} CalibDb_ExpGainMode_t;

typedef struct CalibDb_AecGainRange_s {
    bool IsLinear;
    int array_size;
    float pGainRange[CALD_AEC_GAIN_RANGE_MAX_LEN];
    CalibDb_ExpGainMode_t GainMode;
} CalibDb_AecGainRange_t;

typedef struct CalibDb_Sensor_Para_s {
    //Exp RealValue 2 RegValue
    CalibDb_AecGainRange_t GainRange;
    float TimeFactor[4];
    //ExpSeperateCtrl
    Cam2x1FloatMatrix_t CISHdrTimeRegSumFac;
    Cam2x1FloatMatrix_t CISLinTimeRegMaxFac;
    Cam2x1FloatMatrix_t CISTimeRegOdevity;
    Cam2x1FloatMatrix_t CISHdrTimeRegOdevity;
    uint16_t CISTimeRegMin;
    uint16_t CISHdrTimeRegMin;
    Cam1x3ShortMatrix_t CISHdrTimeRegMax;
    uint8_t CISTimeRegUnEqualEn;
    float CISMinFps;
    CalibDb_AeRange_t CISAgainRange;       //sensor Again or LCG range
    CalibDb_AeRange_t CISExtraAgainRange;  //add for HDR-DCG MODE, HCG range
    CalibDb_AeRange_t CISDgainRange;       //sensor Dgain
    CalibDb_AeRange_t CISIspDgainRange;    //Isp Dgain
    //bit 0: for hdr gain should use the same value;
    //bit 1: support use different gain value
    uint8_t CISHdrGainIndSetEn;  //only used for hdr-stagger mode
    // bit 0 : mirror
    // bit 1 : flip
    uint8_t flip;  // this will change the sensor output image orientation
} CalibDb_Sensor_Para_t;

typedef struct CalibDb_Module_Info_s {
    float FNumber;
    float EFL;
    float LensT;
    float IRCutT;
} CalibDb_Module_Info_t;

#define CALD_AWB_LS_NUM_MAX 14
#define CALD_AWB_WINDOW_NUM_MAX 8
#define CALD_AWB_EXCRANGE_NUM_MAX 7
#define CALD_AWB_RRES_NUM_MAX 5
#define CALD_AWB_LV_NUM_MAX 16
#define CALD_AWB_SGC_NUM_MAX 16
#define CALD_AWB_ILLUMINATION_NAME (20U)
#define CALD_AWB_RES_NAME (20U)
#define CALD_AWB_TEMPORAL_GAIN_SIZE_MAX 4
#define CALD_AWB_XY_TYPE_MAX_V201 2
#define CALD_AWB_GRID_NUM_TOTAL 225
#define CALD_AWB_CT_GRID_NUM_MAX 10
#define CALD_AWB_CRI_GRID_NUM_MAX 10
#define CALD_AWB_CT_LV_NUM_MAX 5
#define CALD_AWB_CT_CLIP_GRID_NUM_MAX 14
#define CALD_AWB_LV_NUM_FOR_TOLERANCE 4
#define CALD_AWB_LV_NUM_FOR_RUNINTERVAL 8

typedef struct _CalibDb_ExcRange_s {
    unsigned char domain; /*0uv domain,1 xy domain*/
    unsigned char mode;
    int xu[2];  //left right
    int yv[2];  //top bottom
} CalibDb_ExcRange_t;

typedef struct _CalibDb_Rgb2TcsParam_s {
    float pseudoLuminanceWeight[3];
    float rotationMat[9];
} CalibDb_Rgb2TcsParam_t;

typedef struct CalibDb_Yuv3D_Range_Ill_s {
    int b_uv;               //17+0 s
    int slope_inv_neg_uv;   //9+10s //-1/k
    int slope_factor_uv;    //9+10s // 1/(-1/k+k)
    int slope_ydis;         //9+10s //k
    int b_ydis;             //17+0s
    unsigned char ref_u;    //8u
    unsigned char ref_v;    //8u
    unsigned short dis[6];  //8+4 u
    unsigned char th[6];    //8u
} CalibDb_Yuv3D_Range_Ill_t;

typedef struct CalibDb_Yuv3D_2_Range_Ill_s {
    float thcurve_u[6];
    float thcure_th[6];
    float line[6];
} CalibDb_Yuv3D_2_Range_Ill_t;

typedef struct CalibDb_UV_Range_Ill_s {
    float pu_region[5];
    float pv_region[5];
} CalibDb_UV_Range_Ill_t;

typedef struct CalibDb_Tcs_Range_Ill_s {
    float xleft;
    float xright;
    float ytop;
    float ybottom;
} CalibDb_Tcs_Range_Ill_t;

typedef struct CalibDb_Awb_Light_Info_s {
    char light_name[16];
    CalibDb_UV_Range_Ill_t uvRange_param;
    CalibDb_UV_Range_Ill_t uvRange_param_small;
    CalibDb_Tcs_Range_Ill_t normalRange;
    CalibDb_Tcs_Range_Ill_t bigRange;
    CalibDb_Tcs_Range_Ill_t smallRange;
    CalibDb_Yuv3D_Range_Ill_t yuv3DRange_param;
    CalibDb_Yuv3D_2_Range_Ill_t yuv3D2Range_param;
} CalibDb_Awb_Light_Info_t;

typedef struct CalibDb_Awb_Light_Info2_s {
    char light_name[CALD_AWB_ILLUMINATION_NAME];
    unsigned char staWeight[CALD_AWB_LV_NUM_MAX];
    unsigned int spatialGain_LV_THH;  //threshold for  spatial gain calculation in different illuminant
    unsigned int spatialGain_LV_THL;  //
    unsigned char xyType2Enable;
    float weightCurve_ratio[4];
    float weightCurve_weight[4];
} CalibDb_Awb_Light_Info2_t;

typedef struct CalibDb_StatWindow_s {
    int mode;
    int resNum;
    char resName[CALD_AWB_RRES_NUM_MAX][CALD_AWB_RES_NAME];
    uint16_t window[CALD_AWB_RRES_NUM_MAX][4];
} CalibDb_StatWindow_t;
typedef struct CalibDb_tcs_range_ill_s {
    float NorrangeX[2];  //left right
    float NorrangeY[2];  //top bottom
    float SperangeX[2];
    float SperangeY[2];
    float SmalrangeX[2];
    float SmalrangeY[2];
} CalibDb_tcs_range_ill_t;

typedef struct CalibDb_Awb_Cct_Clip_Cfg_s {
    float outdoor_cct_min;
    int grid_num;
    float cct[CALD_AWB_CT_CLIP_GRID_NUM_MAX];
    float cri_bound_up[CALD_AWB_CT_CLIP_GRID_NUM_MAX];
    float cri_bound_low[CALD_AWB_CT_CLIP_GRID_NUM_MAX];
} CalibDb_Awb_Cct_Clip_Cfg_t;
typedef struct CalibDb_limit_range_s {
    int lumaNum;
    float lumaValue[CALD_AWB_LV_NUM_MAX];
    unsigned short maxR[CALD_AWB_LV_NUM_MAX];
    unsigned short minR[CALD_AWB_LV_NUM_MAX];
    unsigned short maxG[CALD_AWB_LV_NUM_MAX];
    unsigned short minG[CALD_AWB_LV_NUM_MAX];
    unsigned short maxB[CALD_AWB_LV_NUM_MAX];
    unsigned short minB[CALD_AWB_LV_NUM_MAX];
    unsigned short maxY[CALD_AWB_LV_NUM_MAX];
    unsigned short minY[CALD_AWB_LV_NUM_MAX];
} CalibDb_limit_range_t;

typedef struct CalibDb_Awb_Calib_Para_V200_s {
    struct list_head listHead;
    char scene[CALIBDB_MAX_SCENE_NAME_LENGTH];
    unsigned char hdrFrameChooseMode;
    unsigned char hdrFrameChoose;
    bool lscBypEnable;
    bool uvDetectionEnable;
    bool xyDetectionEnable;
    bool yuvDetectionEnable;
    bool wpDiffWeiEnable;
    bool wbGainClipEn;
    bool wbGainDaylightClipEn;
    bool blkWeightEnable;  //the different weight in WP sum
    bool blkStatisticsEnable;
    int lsUsedForYuvDetNum;
    char lsUsedForYuvDet[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    unsigned char dsMode;
    unsigned char blkMeasureMode;
    CalibDb_StatWindow_t measeureWindow;
    bool multiwindow_en;
    unsigned char lightNum;
    char lightName[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    unsigned char doorType[CALD_AWB_LS_NUM_MAX];
    float standardGainValue[CALD_AWB_LS_NUM_MAX][4];  //rggb
    CalibDb_limit_range_t limitRange;
    CalibDb_Rgb2TcsParam_t rgb2tcs_param;
    //several window in pixel domain
    CalibDb_UV_Range_Ill_t uvRange_param[CALD_AWB_LS_NUM_MAX];
    CalibDb_UV_Range_Ill_t uvRange_param_small[CALD_AWB_LS_NUM_MAX];
    CalibDb_tcs_range_ill_t xyRangeLight[CALD_AWB_LS_NUM_MAX];
    CalibDb_Yuv3D_Range_Ill_t yuv3DRange_param[CALD_AWB_LS_NUM_MAX];
    unsigned short multiwindow[CALD_AWB_WINDOW_NUM_MAX][4];  //8  windows in pixel domain ,hOffset,vOffser,hSize,vSize;
    //several winow in uv or xy domain
    CalibDb_ExcRange_t excludeWpRange[CALD_AWB_EXCRANGE_NUM_MAX];
    float spatialGain_H[4];  //spatial gain
    float spatialGain_L[4];
    float temporalDefaultGain[4];
    float ca_targetGain[4];
    //single color
    unsigned short sSelColorNUM;
    unsigned short sIndSelColor[CALD_AWB_SGC_NUM_MAX];
    float sMeanCh[2][CALD_AWB_SGC_NUM_MAX];
    float srGain[CALD_AWB_LS_NUM_MAX];
    float sbGain[CALD_AWB_LS_NUM_MAX];
    unsigned short sIllEstNum;
    char sNameIllEst[CALD_AWB_LS_NUM_MAX][CALD_AWB_ILLUMINATION_NAME];
    float sAlpha;
    float lineRgBg[3];
    float lineRgProjCCT[3];
    //wbgain clip
    CalibDb_Awb_Cct_Clip_Cfg_t cct_clip_cfg;
} CalibDb_Awb_Calib_Para_V200_t;

typedef struct CalibDb_Awb_line_s {
    float a;
    float b;
    float c;
} CalibDb_Awb_line_t;

typedef struct CalibDb_Awb_Cct_Lut_Cfg_Lv_s {
    float lv;
    int ct_grid_num;
    int cri_grid_num;
    float ct_range[2];   //min,max, equal distance sapmle
    float cri_range[2];  //min,max
    float ct_lut_out[CALD_AWB_CT_GRID_NUM_MAX * CALD_AWB_CRI_GRID_NUM_MAX];
    float cri_lut_out[CALD_AWB_CT_GRID_NUM_MAX * CALD_AWB_CRI_GRID_NUM_MAX];
} CalibDb_Awb_Cct_Lut_Cfg_Lv_t;

typedef struct CalibDb_Awb_gain_offset_cfg_s {
    bool enable;
    float offset[4];
} CalibDb_Awb_gain_offset_cfg_t;

typedef struct rk_aiq_wb_awb_runinterval_s {
    int num;
    float LV[CALD_AWB_LV_NUM_FOR_RUNINTERVAL];
    float value[CALD_AWB_LV_NUM_FOR_RUNINTERVAL];
} CalibDb_Awb_runinterval_t;

typedef struct rk_aiq_wb_awb_tolerance_s {
    int num;
    float LV[CALD_AWB_LV_NUM_FOR_RUNINTERVAL];
    float value[CALD_AWB_LV_NUM_FOR_RUNINTERVAL];
} CalibDb_Awb_tolerance_t;

typedef struct CalibDb_Awb_Remosaic_Para_s {
    bool enable;
    float sensor_awb_gain[4];
} CalibDb_Awb_Remosaic_Para_t;

typedef struct CalibDb_Awb_Wp_Th_s {
    int num;
    float lumaValue[CALD_AWB_LV_NUM_MAX];
    float WP_THH[CALD_AWB_LV_NUM_MAX];
    float WP_THL[CALD_AWB_LV_NUM_MAX];
} CalibDb_Awb_Wp_Th_t;

typedef struct CalibDb_Awb_XyReionSelect_WpNumTh_s {
    int num;
    float lumaValue[CALD_AWB_LV_NUM_MAX];
    float wpNumThForBigType[CALD_AWB_LV_NUM_MAX];
    float wpNumThForExtraType[CALD_AWB_LV_NUM_MAX];
} CalibDb_Awb_XyReionSelect_WpNumTh_t;

typedef struct CalibDb_Awb_Adjust_Para_s {
    struct list_head listHead;
    char scene[CALIBDB_MAX_SCENE_NAME_LENGTH];
    bool wbBypass;
    bool awbEnable;
    unsigned char lightNum;
    bool ca_enable;
    bool wbGainAdjustEn;
    char lsForFirstFrame[CALD_AWB_ILLUMINATION_NAME];
    //multiwindow
    unsigned char multiwindowMode;
    bool uvRange_small_enable;
    CalibDb_Awb_tolerance_t tolerance;  //wb gain diff th for awb gain update, set 0 to disable this function
    CalibDb_Awb_runinterval_t runInterval;
    //wbgain damp
    float dFStep;
    float dFMin;
    float dFMax;
    int LvIIRsize;
    float LvVarTh;
    int LV_NUM;
    unsigned int LVMatrix[CALD_AWB_LV_NUM_MAX];
    unsigned int LV_THL;
    unsigned int LV_THL2;
    unsigned int LV_THH;
    unsigned int LV_THH2;
    CalibDb_Awb_Wp_Th_t WP_TH;
    float proDis_THL;  //threshold for distance probality calculation
    float proDis_THH;
    unsigned int proLV_Indoor_THL;  // threshold for luminance probality calculation
    unsigned int proLV_Indoor_THH;
    unsigned int proLV_Outdoor_THL;
    unsigned int proLV_Outdoor_THH;
    unsigned char temporalCalGainSetSize;
    unsigned char temporalGainSetWeight
        // ËÄÖ¡µÄ±ÈÀý0-100×ÜºÍÎª100//gainPer[0]Îª-1Ö¡£¬gainPer[1]is -2,gainPer[2] is -3 gainPer[3] is -4
        [CALD_AWB_TEMPORAL_GAIN_SIZE_MAX];
    float wpNumPercTh;  //ÎÞÐ§°×µããÐÖµ£¬°×µãÊýÁ¿ÉÙÊ±ÈÏÎª¸Ã°×µãÎÞÐ§
    unsigned char tempWeight[CALD_AWB_LV_NUM_MAX];
    //color adaptation
    float ca_LACalcFactor;
    //wb gain shift 2 //to do  from xml
    int cct_lut_cfg_num;
    CalibDb_Awb_Cct_Lut_Cfg_Lv_t cct_lut_cfg[CALD_AWB_CT_LV_NUM_MAX];
    CalibDb_Awb_gain_offset_cfg_t wbGainOffset;
    float convergedVarTh;
    //make  xyTypeSelect stable
    int xyTypeListSize;  // xyTypeListSize ==0 will disable this function
    float varianceLumaTh;
    CalibDb_Awb_XyReionSelect_WpNumTh_t wpNumTh;
    CalibDb_Awb_Light_Info2_t awb_light_info[CALD_AWB_LS_NUM_MAX];
    bool xyType2ForColBalEnable;  // to do for awb2.1
    CalibDb_Awb_Remosaic_Para_t remosaic_cfg;
} CalibDb_Awb_Adjust_Para_t;

typedef struct CalibDb_Awb_Para_s {
    CalibDb_Awb_Calib_Para_V200_t calib_para_v200;
    CalibDb_Awb_Adjust_Para_t adjust_para;
} CalibDb_Awb_Para_t;

#endif
