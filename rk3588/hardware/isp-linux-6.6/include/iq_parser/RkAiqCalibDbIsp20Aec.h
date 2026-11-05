#ifndef RK_AIQ_CALIB_DB_ISP20_AEC_H
#define RK_AIQ_CALIB_DB_ISP20_AEC_H

#ifndef _RK_AIQ_CALIB_TYPES_ISP20_H_
#error "Include RkAiqCalibDbTypesIsp20.h first"
#endif

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC Algo Params
 */
/*****************************************************************************/

#define AEC_ECM_SCHEME_NAME (20U)
typedef char AecEcmSchemeName_t[AEC_ECM_SCHEME_NAME];

#define AEC_ECM_PROFILE_NAME (20U)
typedef char AecEcmProfileName_t[AEC_ECM_PROFILE_NAME];

#define AEC_DYNAMIC_SETPOINT_NAME (20U)
typedef char AecDynamicSetpointName_t[AEC_DYNAMIC_SETPOINT_NAME];

#define AEC_EXP_SEPARATE_NAME (20U)
typedef char AecExpSeparateName_t[AEC_EXP_SEPARATE_NAME];

#define AEC_RAWAEBIG_WIN_NUM (15 * 15)
#define AEC_RAWAELITE_WIN_NUM (5 * 5)

typedef enum _CalibDb_HdrAeRatioType_e {
    RKAIQ_HDRAE_RATIOTYPE_MODE_INVALID = 0,
    RKAIQ_HDRAE_RATIOTYPE_MODE_AUTO = 1,
    RKAIQ_HDRAE_RATIOTYPE_MODE_FIX = 2,
    RKAIQ_HDRAE_RATIOTYPE_MODE_MAX
} CalibDb_HdrAeRatioType_t;

typedef enum _CalibDb_AeStrategyMode_e {
    RKAIQ_AEC_STRATEGY_MODE_AUTO = 0,
    RKAIQ_AEC_STRATEGY_MODE_LOWLIGHT_PRIOR = 1,
    RKAIQ_AEC_STRATEGY_MODE_HIGHLIGHT_PRIOR = 2,
    RKAIQ_AEC_STRATEGY_MODE_MAX
} CalibDb_AeStrategyMode_t;

typedef enum _CalibDb_AeHdrLongFrmMode_e {
    RKAIQ_AEC_HDR_LONGFRMMODE_NORMAL = 0,
    RKAIQ_AEC_HDR_LONGFRMMODE_AUTO_LONG_FRAME = 1,
    RKAIQ_AEC_HDR_LONGFRMMODE_LONG_FRAME = 2,
} CalibDb_AeHdrLongFrmMode_t;

typedef enum _CalibDb_AecDayNightMode_e {
    AEC_DNMODE_MIN = -1,
    AEC_DNMODE_DAY = 0,
    AEC_DNMODE_NIGHT = 1, /*night mode or IR night-vision mode*/
    AEC_DNMODE_MAX = 2,
} CalibDb_AecDayNightMode_t;

/*****************************************************************************/
/**
 *          AecSemMode_t
 *
 * @brief   mode type of AEC Scene Evaluation
 *
 */
/*****************************************************************************/
typedef enum _CalibDb_AecSemMode_e {
    AEC_SCENE_EVALUATION_INVALID = 0,  /* invalid (only used for initialization) */
    AEC_SCENE_EVALUATION_DISABLED = 1, /* Scene Evaluation disabled (fix setpoint) */
    AEC_SCENE_EVALUATION_FIX = 2,      /* Scene Evaluation fix (static ROI) */
    AEC_SCENE_EVALUATION_ADAPTIVE = 3, /* Scene Evaluation adaptive (ROI caluclated by Scene Evaluation */
    AEC_SCENE_EVALUATION_MAX
} CalibDb_AecSemMode_t;
/*****************************************************************************/
/**
 *          AecBackLitMode_t
 *
 * @brief   mode type of AEC-BackLit
 *
 */
/*****************************************************************************/
typedef enum _CalibDb_AecMeasAreaMode_e {
    AEC_MEASURE_AREA_AUTO = 0,
    AEC_MEASURE_AREA_UP,
    AEC_MEASURE_AREA_BOTTOM,
    AEC_MEASURE_AREA_LEFT,
    AEC_MEASURE_AREA_RIGHT,
    AEC_MEASURE_AREA_CENTER,
} CalibDb_AecMeasAreaMode_t;

/*****************************************************************************/
typedef enum _CalibDb_AecECMMode_e {
    AEC_ECM_MODE_INVALID = 0, /**< invalid exposure conversion module */
    AEC_ECM_MODE_1 = 1,       /* the original exposure conversion module ,   minimize the value of gain   */
    AEC_ECM_MODE_2 = 2,       /* the newl exposure conversion module ,imcrease the frame rate */
    AEC_ECM_MODE_MAX,
} CalibDb_AecECMMode_t;

typedef enum _CalibDb_FlickerFreq_e {
    AEC_FLICKER_FREQUENCY_OFF = 0,
    AEC_FLICKER_FREQUENCY_50HZ = 1,
    AEC_FLICKER_FREQUENCY_60HZ = 2,
} CalibDb_FlickerFreq_t;

typedef enum _CalibDb_AntiFlickerMode_e {
    AEC_ANTIFLICKER_NORMAL_MODE = 0,
    AEC_ANTIFLICKER_AUTO_MODE = 1,
} CalibDb_AntiFlickerMode_t;

typedef enum _CalibDb_IrisType_e {
    IRIS_DC_TYPE = 0,
    IRIS_P_TYPE = 1,
    IRIS_HDC_TYPE = 2,
    IRIS_INVALID_TYPE,
} CalibDb_IrisType_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to configure CamerIC ISP exposure measuring mode.
 *
 *****************************************************************************/
typedef enum _CalibDb_CamYRangeMode_e {
    CAM_YRANGE_MODE_INVALID = 0, /**< invalid y range mode   */
    CAM_YRANGE_MODE_FULL = 1,    /**< Y = 0.299 * R + 0.587 * G + 0.114 * B,full range BT601*/
    CAM_YRANGE_MODE_LIMITED = 2, /**< Y = 16 + 0.25R + 0.5G + 0.1094B,limited range*/
    CAM_YRANGE_MODE_MAX,
} CalibDb_CamYRangeMode_t;

typedef enum _CalibDb_CamRawStatsMode_e {
    CAM_RAWSTATS_MODE_INVALID = 0, /**< lower border (only for an internal evaluation) */
    CAM_RAWSTATS_MODE_R = 1,       /**< R channel */
    CAM_RAWSTATS_MODE_G = 2,       /**< G channel */
    CAM_RAWSTATS_MODE_B = 3,       /**< B channel */
    CAM_RAWSTATS_MODE_Y = 4,       /**< luminance channel */
    CAM_RAWSTATS_MODE_MAX,         /**< upper border (only for an internal evaluation) */
} CalibDb_CamRawStatsMode_t;

typedef enum _CalibDb_CamHistStatsMode_e {
    CAM_HIST_MODE_INVALID = 0,      /**< lower border (only for an internal evaluation) */
    CAM_HIST_MODE_RGB_COMBINED = 1, /**< RGB combined histogram, only available for SiHist */
    CAM_HIST_MODE_R = 2,            /**< R histogram */
    CAM_HIST_MODE_G = 3,            /**< G histogram */
    CAM_HIST_MODE_B = 4,            /**< B histogram */
    CAM_HIST_MODE_Y = 5,            /**< Y luminance histogram */
    CAM_HIST_MODE_MAX,              /**< upper border (only for an internal evaluation) */
} CalibDb_CamHistStatsMode_t;

typedef struct CalibDb_Aec_Win_s {
    unsigned short h_offs;
    unsigned short v_offs;
    unsigned short h_size;
    unsigned short v_size;
} CalibDb_Aec_Win_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API Exp-Route Params
 */
/*****************************************************************************/
#define AEC_ROUTE_MAX_NODES (10)
typedef struct CalibDb_LinAeRoute_Attr_s {
    AecExpSeparateName_t name; /**name */
    float TimeDot[AEC_ROUTE_MAX_NODES];
    float GainDot[AEC_ROUTE_MAX_NODES];
    float IspgainDot[AEC_ROUTE_MAX_NODES];
    int PIrisGainDot[AEC_ROUTE_MAX_NODES];
    int array_size;
} CalibDb_LinAeRoute_Attr_t;

typedef struct CalibDb_HdrAeRoute_Attr_s {
    AecExpSeparateName_t name; /**name */
    float HdrTimeDot[3][AEC_ROUTE_MAX_NODES];
    float HdrGainDot[3][AEC_ROUTE_MAX_NODES];
    float HdrIspDGainDot[3][AEC_ROUTE_MAX_NODES];
    int PIrisGainDot[AEC_ROUTE_MAX_NODES];
    int array_size;
} CalibDb_HdrAeRoute_Attr_t;

typedef struct CalibDb_AeRoute_Attr_s {
    CalibDb_LinAeRoute_Attr_t LinAeSeperate[AEC_DNMODE_MAX];
    CalibDb_HdrAeRoute_Attr_t HdrAeSeperate[AEC_DNMODE_MAX];
} CalibDb_AeRoute_Attr_t;

typedef struct CalibDb_AeEnvLvCalib_s {
    float CalibFN;
    float RealFN;
    Cam2x1FloatMatrix_t Curve;
} CalibDb_AeEnvLvCalib_t;

//2). Auto exposure
typedef struct CalibDb_AeSpeed_s {
    bool SmoothEn;
    bool DyDampEn;
    float DampOver;
    float DampUnder;
    float DampDark2Bright;
    float DampBright2Dark;
} CalibDb_AeSpeed_t;

typedef struct CalibDb_AeRange_s {
    float Min;
    float Max;
} CalibDb_AeRange_t;

typedef struct CalibDb_LinAeRange_s {
    CalibDb_AeRange_t stExpTimeRange;
    CalibDb_AeRange_t stGainRange;
    CalibDb_AeRange_t stIspDGainRange;
    CalibDb_AeRange_t stPIrisRange;
} CalibDb_LinAeRange_t;

typedef struct CalibDb_HdrAeRange_s {
    CalibDb_AeRange_t stExpTimeRange[3];
    CalibDb_AeRange_t stGainRange[3];
    CalibDb_AeRange_t stIspDGainRange[3];
    CalibDb_AeRange_t stPIrisRange;
} CalibDb_HdrAeRange_t;

typedef struct CalibDb_AeFrmRateAttr_s {
    bool isFpsFix;
    float FpsValue;
} CalibDb_AeFrmRateAttr_t;

typedef struct CalibDb_AntiFlickerAttr_s {
    bool enable;
    CalibDb_FlickerFreq_t Frequency;
    CalibDb_AntiFlickerMode_t Mode;
} CalibDb_AntiFlickerAttr_t;

//3.) Init expsore
typedef struct CalibDb_LinExpInitExp_s {
    float InitTimeValue;
    float InitGainValue;
    float InitIspDGainValue;
    int InitPIrisGainValue;
    int InitDCIrisDutyValue;
    int InitHDCIrisTargetValue;
    int array_size;
} CalibDb_LinExpInitExp_t;

typedef struct CalibDb_HdrExpInitExp_s {
    Cam3x1FloatMatrix_t InitTimeValue;
    Cam3x1FloatMatrix_t InitGainValue;
    Cam3x1FloatMatrix_t InitIspDGainValue;
    int InitPIrisGainValue;
    int InitDCIrisDutyValue;
    int InitHDCIrisTargetValue;
    int array_size;
} CalibDb_HdrExpInitExp_t;

typedef struct CalibDb_ExpInitExp_s {
    CalibDb_LinExpInitExp_t stLinExpInitExp;
    CalibDb_HdrExpInitExp_t stHdrExpInitExp;
} CalibDb_ExpInitExp_t;

typedef struct CalibDb_AeAttr_s {
    CalibDb_AeSpeed_t stAeSpeed;
    //DelayFrmNum
    uint8_t BlackDelayFrame;
    uint8_t WhiteDelayFrame;
    //AeRange
    bool SetAeRangeEn;
    CalibDb_LinAeRange_t stLinAeRange;
    CalibDb_HdrAeRange_t stHdrAeRange;
    //Auto/Fixed fps
    CalibDb_AeFrmRateAttr_t stFrmRate;
} CalibDb_AeAttr_t;
//manual exposure
typedef struct CalibDb_LinMeAttr_s {
    bool ManualTimeEn;
    bool ManualGainEn;
    bool ManualIspDgainEn;
    bool ManualIrisEn;
    float TimeValue;
    float GainValue;
    float IspDGainValue;
    int PIrisGainValue;
    int DCIrisValue;
    int HDCIrisValue;
} CalibDb_LinMeAttr_t;

typedef struct CalibDb_HdrMeAttr_s {
    bool ManualTimeEn;
    bool ManualGainEn;
    bool ManualIspDgainEn;
    bool ManualIrisEn;
    Cam3x1FloatMatrix_t TimeValue;
    Cam3x1FloatMatrix_t GainValue;
    Cam3x1FloatMatrix_t IspDGainValue;
    int PIrisGainValue;
    int DCIrisValue;
    int HDCIrisValue;
} CalibDb_HdrMeAttr_t;

typedef struct CalibDb_MeAttr_s {
    CalibDb_LinMeAttr_t stLinMe;
    CalibDb_HdrMeAttr_t stHdrMe;
} CalibDb_MeAttr_t;

// DayorNight Switch
typedef struct CalibDb_IRNightMode_s {
    uint8_t enable;
    float IRRgain;
    float IRBgain;
    float MaxWbDis;
    uint8_t Night2DayFrmCnt;
    float Night2DayFacTh;
    float VbPercent;
} CalibDb_IRNightMode_t;

typedef struct CalibDb_VBNightMode_s {
    uint8_t enable;
    float Night2DayFacTh;
    uint8_t Night2DayFrmCnt;
} CalibDb_VBNightMode_t;

typedef struct CalibDb_DNSwitch_Attr_s {
    uint8_t DNTrigger;
    CalibDb_AecDayNightMode_t DNMode;
    uint8_t FillLightMode;
    float Day2NightFacTh;
    uint8_t Day2NightFrmCnt;
    CalibDb_VBNightMode_t stVBNightMode;
    CalibDb_IRNightMode_t stIRNightMode;
} CalibDb_DNSwitch_Attr_t;

//Aec-Sync-Test used for debug
#define AEC_ALTER_EXP_MAX_NUM (10)

typedef struct CalibDb_LinAlterExp_s {
    float TimeValue[AEC_ALTER_EXP_MAX_NUM];
    float GainValue[AEC_ALTER_EXP_MAX_NUM];
    float IspgainValue[AEC_ALTER_EXP_MAX_NUM];
    int PIrisGainValue[AEC_ALTER_EXP_MAX_NUM];
    int DcgMode[AEC_ALTER_EXP_MAX_NUM];
    int array_size;
} CalibDb_LinAlterExp_t;

typedef struct CalibDb_HdrAlterExp_s {
    float TimeValue[AEC_ALTER_EXP_MAX_NUM][3];
    float GainValue[AEC_ALTER_EXP_MAX_NUM][3];
    float IspDGainValue[AEC_ALTER_EXP_MAX_NUM][3];
    int PIrisGainValue[AEC_ALTER_EXP_MAX_NUM];
    int DcgMode[AEC_ALTER_EXP_MAX_NUM][3];
    int array_size;
} CalibDb_HdrAlterExp_t;

typedef struct CalibDb_AeSyncTest_s {
    uint8_t enable;
    int IntervalFrm;
    CalibDb_LinAlterExp_t LinAlterExp;
    CalibDb_HdrAlterExp_t HdrAlterExp;
} CalibDb_AeSyncTest_t;

//AecIrisCtrl
#define AEC_PIRIS_STAP_TABLE_MAX (1024)
typedef struct CalibDb_PIris_Attr_s {
    uint16_t TotalStep;
    uint16_t EffcStep;
    bool ZeroIsMax;
    uint16_t StepTable[AEC_PIRIS_STAP_TABLE_MAX];
} CalibDb_PIris_Attr_t;

typedef struct CalibDb_DCIris_Attr_s {
    float Kp;
    float Ki;
    float Kd;
    int MinPwmDuty;
    int MaxPwmDuty;
    int OpenPwmDuty;
    int ClosePwmDuty;
} CalibDb_DCIris_Attr_t;

#define AEC_HDCIRIS_DOT_MAX_NUM (256)
typedef struct CalibDb_HDCIris_Attr_s {
    float DampOver;
    float DampUnder;
    bool ZeroIsMax;
    int MinTarget;
    int MaxTarget;
    int ZoomTargetDot[AEC_HDCIRIS_DOT_MAX_NUM];
    int ZoomDot[AEC_HDCIRIS_DOT_MAX_NUM];
    int IrisTargetDot[AEC_HDCIRIS_DOT_MAX_NUM];
    int GainDot[AEC_HDCIRIS_DOT_MAX_NUM];
    int zoom_array_size;
    int iris_array_size;
} CalibDb_HDCIris_Attr_t;

typedef struct CalibDb_AecIrisCtrl_s {
    uint8_t enable;
    CalibDb_IrisType_t IrisType;
    CalibDb_PIris_Attr_t PIrisAttr;
    CalibDb_DCIris_Attr_t DCIrisAttr;
    CalibDb_HDCIris_Attr_t HDCIrisAttr;
} CalibDb_AecIrisCtrl_t;

typedef struct CalibDb_AecCommon_Attr_s {
    uint8_t enable;
    uint8_t AecRunInterval;
    RKAiqOPMode_t AecOpType;
    CalibDb_CamRawStatsMode_t RawStatsMode;
    CalibDb_CamHistStatsMode_t HistStatsMode;
    CalibDb_CamYRangeMode_t YRangeMode;
    //GridWeight
    Cam15x15UCharMatrix_t DayGridWeights;
    int DayWeightNum;
    Cam15x15UCharMatrix_t NightGridWeights;
    int NightWeightNum;
    CalibDb_AecIrisCtrl_t stIris;
    CalibDb_AeSyncTest_t stSyncTest;
    //envlv calibration
    CalibDb_AeEnvLvCalib_t stEnvLvCalib;
    //antiflicker
    CalibDb_AntiFlickerAttr_t stAntiFlicker;
    //initial exp
    CalibDb_ExpInitExp_t stInitExp;
    //DayOrNight switch
    CalibDb_DNSwitch_Attr_t stDNSwitch;
    //ExpSeperate
    CalibDb_AeRoute_Attr_t stAeRoute;
    //manual expvalue
    CalibDb_MeAttr_t stManual;
    CalibDb_AeAttr_t stAuto;
} CalibDb_AecCommon_Attr_t;

/*****************************************************************************/
/**
 * @brief   ISP2.0 AEC API algo Config Params(private)
 */
/*****************************************************************************/

#ifndef AEC_SETPOINT_MAX_NODES
#define AEC_SETPOINT_MAX_NODES 10
#endif
typedef struct CalibDb_AecDynamicSetpoint_s {
    AecDynamicSetpointName_t name; /**name */
    float ExpValue[AEC_SETPOINT_MAX_NODES];
    float DySetpoint[AEC_SETPOINT_MAX_NODES];
    int array_size;
} CalibDb_AecDynamicSetpoint_t;

typedef struct CalibDb_AecBacklight_s {
    uint8_t enable;
    float StrBias;
    CalibDb_AecMeasAreaMode_t MeasArea;
    float OEROILowTh;
    float LumaDistTh;
    float LvLowTh;
    float LvHightTh;
    Cam1x6FloatMatrix_t ExpLevel;
    Cam1x6FloatMatrix_t NonOEPdfTh;
    Cam1x6FloatMatrix_t LowLightPdfTh;
    Cam1x6FloatMatrix_t TargetLLLuma;
} CalibDb_AecBacklight_t;

typedef struct CalibDb_AecOverExpCtrl_s {
    uint8_t enable;
    float StrBias;
    float MaxWeight;
    float HighLightTh;
    float LowLightTh;
    Cam1x6FloatMatrix_t OEpdf;
    Cam1x6FloatMatrix_t LowLightWeight;
    Cam1x6FloatMatrix_t HighLightWeight;
} CalibDb_AecOverExpCtrl_t;

typedef struct CalibDb_LinearAE_Attr_s {
    uint8_t RawStatsEn;
    float SetPoint; /**< set point to hit by the ae control system */
    float NightSetPoint;
    float ToleranceIn;
    float ToleranceOut;
    float Evbias;
    CalibDb_AeStrategyMode_t StrategyMode;
    bool DySetPointEn;
    CalibDb_AecDynamicSetpoint_t DySetpoint[AEC_DNMODE_MAX];
    CalibDb_AecBacklight_t BackLightConf;
    CalibDb_AecOverExpCtrl_t OverExpCtrl;
} CalibDb_LinearAE_Attr_t;

typedef struct CalibDb_LFrameCtrl_s {
    //LframeCtrl
    float OEROILowTh;
    float LvLowTh;
    float LvHighTh;
    Cam1x6FloatMatrix_t LExpLevel;
    Cam1x6FloatMatrix_t LSetPoint;
    Cam1x6FloatMatrix_t TargetLLLuma;
    Cam1x6FloatMatrix_t NonOEPdfTh;
    Cam1x6FloatMatrix_t LowLightPdfTh;
} CalibDb_LFrameCtrl_t;

typedef struct CalibDb_MFrameCtrl_s {
    //MframeCtrl
    Cam1x6FloatMatrix_t MExpLevel;
    Cam1x6FloatMatrix_t MSetPoint;
} CalibDb_MFrameCtrl_t;

typedef struct CalibDb_SFrameCtrl_s {
    //SframeCtrl
    float HLLumaTolerance;
    Cam1x6FloatMatrix_t SExpLevel;
    Cam1x6FloatMatrix_t TargetHLLuma;
    Cam1x6FloatMatrix_t SSetPoint;
    bool HLROIExpandEn;
} CalibDb_SFrameCtrl_t;

typedef struct CalibDb_HdrAE_Attr_s {
    float ToleranceIn;
    float ToleranceOut;
    CalibDb_AeHdrLongFrmMode_t LongfrmMode;
    uint16_t SfrmMinLine;
    float LfrmModeExpTh;
    CalibDb_AeStrategyMode_t StrategyMode;
    float Evbias;
    //ExpRatioCtrl
    CalibDb_HdrAeRatioType_t ExpRatioType;
    Cam1x6FloatMatrix_t RatioExpDot;
    Cam1x6FloatMatrix_t M2SRatioFix;
    Cam1x6FloatMatrix_t L2MRatioFix;
    Cam1x6FloatMatrix_t M2SRatioMax;
    Cam1x6FloatMatrix_t L2MRatioMax;
    float LumaDistTh;  //used for area-growing
    CalibDb_LFrameCtrl_t LframeCtrl;
    CalibDb_MFrameCtrl_t MframeCtrl;
    CalibDb_SFrameCtrl_t SframeCtrl;
} CalibDb_HdrAE_Attr_t;

/*****************************************************************************/
/**
 * @brief   Global AEC calibration structure of isp2.0
 */
/*****************************************************************************/
typedef struct CalibDb_Aec_Para_s {
    CalibDb_AecCommon_Attr_t CommCtrl;
    CalibDb_LinearAE_Attr_t LinearAeCtrl;
    CalibDb_HdrAE_Attr_t HdrAeCtrl;
} CalibDb_Aec_Para_t;

#endif
