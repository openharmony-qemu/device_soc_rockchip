/*
 * Copyright 2020 Rockchip Electronics Co. LTD
 *
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

#ifndef __MPP_CFG_H__
#define __MPP_CFG_H__

#include "rk_type.h"
#include "mpp_err.h"

typedef enum CfgType_e {
    CfgTypeS32,
    CfgTypeU32,
    CfgTypeS64,
    CfgTypeU64,
    CfgTypeSt,
    CfgTypePtr,
    CfgTypeBUTT,
} CfgType;

typedef struct MppCfgInfo_t {
    CfgType data_type;
    /* update flag info 32bit */
    RK_U32 flag_type;
    RK_U32 flag_offset;
    RK_U32 flag_value;
    /* data access info */
    RK_U32 data_offset;
    RK_S32 data_size;
} MppCfgInfo;

MPP_RET mpp_cfg_set_s32(MppCfgInfo *info, void *cfg, RK_S32 val);
MPP_RET mpp_cfg_get_s32(MppCfgInfo *info, void *cfg, RK_S32 *val);
MPP_RET mpp_cfg_set_u32(MppCfgInfo *info, void *cfg, RK_U32 val);
MPP_RET mpp_cfg_get_u32(MppCfgInfo *info, void *cfg, RK_U32 *val);
MPP_RET mpp_cfg_set_s64(MppCfgInfo *info, void *cfg, RK_S64 val);
MPP_RET mpp_cfg_get_s64(MppCfgInfo *info, void *cfg, RK_S64 *val);
MPP_RET mpp_cfg_set_u64(MppCfgInfo *info, void *cfg, RK_U64 val);
MPP_RET mpp_cfg_get_u64(MppCfgInfo *info, void *cfg, RK_U64 *val);
MPP_RET mpp_cfg_set_st(MppCfgInfo *info, void *cfg, void *val);
MPP_RET mpp_cfg_get_st(MppCfgInfo *info, void *cfg, void *val);
MPP_RET mpp_cfg_set_ptr(MppCfgInfo *info, void *cfg, void *val);
MPP_RET mpp_cfg_get_ptr(MppCfgInfo *info, void *cfg, void **val);

/* header size 128 byte */
typedef struct MppCfgInfoHead_t {
    char version[116];
    RK_S32 info_size;
    RK_S32 info_count;
    RK_S32 node_count;
} MppCfgInfoHead;

#ifdef __cplusplus
extern "C" {
#endif

const char *strof_cfg_type(CfgType type);

MPP_RET CHECK_CFG_INFO(MppCfgInfo *node, const char *name, CfgType type, const char *func);

MPP_RET check_cfg_info(MppCfgInfo *node, const char *name, CfgType type, const char *func);

#ifdef __cplusplus
}
#endif

#endif /* __MPP_CFG_H__ */
