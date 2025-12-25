/*
 * Copyright 2015 Rockchip Electronics Co. LTD
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

#define LOG_TAG "PpOp"

#include "vpu.h"
#include "ppOp.h"
#include <securec.h>

namespace ppOp  {
status_t ppOpInit(PP_OP_HANDLE *hnd, PP_OPERATION *init)
{
    (void)hnd;
    (void)init;
    return 0;
}

status_t ppOpSet(PP_OP_HANDLE hnd, PP_SET_OPT opt, RK_U32 val)
{
    (void)hnd;
    (void)opt;
    (void)val;
    return 0;
}

status_t ppOpPerform(PP_OP_HANDLE hnd)
{
    (void)hnd;
    return 0; // VPU_REG_NUM_PP);
}

status_t ppOpSync(PP_OP_HANDLE hnd)
{
    (void)hnd;
    return 0;
}

status_t ppOpRelease(PP_OP_HANDLE hnd)
{
    (void)hnd;
    return 0;
}
}

#if BUILD_PPOP_TEST
#define SRC_WIDTH (1920)  // (1920)
#define SRC_HEIGHT (1080) // (1080)
#define SRC_SIZE (SRC_WIDTH * SRC_HEIGHT * 2)

#define DST_WIDTH (720) // (720)
#define DST_HEIGHT (1280)
#define DST_SIZE (DST_WIDTH * DST_HEIGHT * 2)
#define ALIGN_16 15

#include "vpu_mem.h"
#include "vpu.h"
#include "ppOp.h"

const int BIT_MASK_ALIGN_16 = 0xf;
const int ALIGN_16_VALUE = 15;

const int YUV420_CHROMA_SIZE_DIVISOR = 2;
const int YUV420_TOTAL_SIZE_MULTIPLIER = 3;
const int YUV420_TOTAL_SIZE_DIVISOR = 2;
const int MEMORY_ALLOCATION_MULTIPLIER = 2;
const int QUARTER_DIVISOR = 4;

const int SRC_VIR_WIDTH = 1920;
const int SRC_VIR_HEIGHT = 1088;
const int DST_VIR_WIDTH = 800;
const int DST_VIR_HEIGHT = 1280;

static int initVpuResources(VPUMemLinear_t *src, VPUMemLinear_t *dst, int *vpuFd)
{
    int ret = 0;
    // 分配VPU内存
    ret |= VPUMallocLinear(src, SRC_VIR_WIDTH * SRC_VIR_HEIGHT * MEMORY_ALLOCATION_MULTIPLIER);
    ret |= VPUMallocLinear(dst, DST_VIR_WIDTH * DST_VIR_HEIGHT * MEMORY_ALLOCATION_MULTIPLIER);
    if (ret) {
        ALOGE("failed to malloc vpu_mem");
        return ret;
    }
    // 初始化VPU客户端
    *vpuFd = VPUClientInit(VPU_PP);
    if (*vpuFd < 0) {
        ALOGE("failed to open vpu client");
        VPUFreeLinear(src);
        VPUFreeLinear(dst);
        return -1;
    }
    return 0;
}

static int readYuvFile(void *srcVirAddr)
{
    FILE *fpr = fopen("/sdcard/testin.yuv", "rb");
    if (!fpr) {
        ALOGE("failed to open testin.yuv");
        return -1;
    }

    char *tmp = (char *)srcVirAddr;
    // 读取Y分量
    for (int i = 0; i < SRC_HEIGHT; i++) {
        tmp += fread(tmp, 1, SRC_WIDTH, fpr);
        // tmp += SRC_VIR_WIDTH;
    }

    // 读取UV分量
    tmp = (char *)srcVirAddr;
    int nRead = fread(&tmp[SRC_VIR_WIDTH * SRC_VIR_HEIGHT], 1,
                      SRC_WIDTH * SRC_HEIGHT / YUV420_CHROMA_SIZE_DIVISOR, fpr);
    if (nRead < 0) {
        ALOGE("fread return error,ret = %d", nRead);
    }
    if (fclose(fpr) != 0) {
        ALOGE("fclose  error");
    }
    return 0;
}

static int convertPlanarToSemiPlanar(void *srcVirAddr, char *tmpbuf)
{
    char *tmp = (char *)srcVirAddr;
    
    for (int i = 0; i < SRC_HEIGHT / YUV420_CHROMA_SIZE_DIVISOR; i++) {
        for (int j = 0; j < SRC_WIDTH / YUV420_CHROMA_SIZE_DIVISOR; j++) {
            tmpbuf[i * SRC_VIR_WIDTH + j * MEMORY_ALLOCATION_MULTIPLIER + 0] =
                tmp[SRC_VIR_WIDTH * SRC_VIR_HEIGHT + i * SRC_WIDTH / YUV420_CHROMA_SIZE_DIVISOR + j];
            tmpbuf[i * SRC_VIR_WIDTH + j * MEMORY_ALLOCATION_MULTIPLIER + 1] =
                tmp[SRC_VIR_WIDTH * SRC_VIR_HEIGHT + SRC_WIDTH * SRC_HEIGHT / QUARTER_DIVISOR +
                    i * SRC_WIDTH / YUV420_CHROMA_SIZE_DIVISOR + j];
        }
    }

    RK_S32 ret = memcpy_s(&tmp[SRC_VIR_WIDTH * SRC_VIR_HEIGHT],
                          SRC_VIR_WIDTH * SRC_VIR_HEIGHT / YUV420_CHROMA_SIZE_DIVISOR,
                          tmpbuf, SRC_VIR_WIDTH * SRC_VIR_HEIGHT / YUV420_CHROMA_SIZE_DIVISOR);
    if (ret != EOK) {
        ALOGE("copy data failed, error code: %d", ret);
        return -1;
    }
    return 0;
}

static int processVpuPP(VPUMemLinear_t *src, VPUMemLinear_t *dst, int vpuFd)
{
    int ret = 0;
    int wid_alig16;
    int hei_alig16;
    ppOp::PP_OP_HANDLE hnd = nullptr;
    ppOp::PP_OPERATION opt;

    ret = memset_s(&opt, sizeof(opt), 0, sizeof(opt));
    if (ret != EOK) {
        ALOGE("memset_s opt failed, error code: %d", ret);
        return ret;
    }

    wid_alig16 = ((SRC_WIDTH + ALIGN_16_VALUE) & (~BIT_MASK_ALIGN_16));
    hei_alig16 = ((SRC_HEIGHT + ALIGN_16_VALUE) & (~BIT_MASK_ALIGN_16));
    opt.srcAddr = src->phy_addr;
    opt.srcFormat = PP_IN_FORMAT_YUV420SEMI;
    opt.srcWidth = SRC_WIDTH;
    opt.srcHeight = SRC_HEIGHT;
    opt.srcHStride = SRC_VIR_WIDTH;
    opt.srcVStride = SRC_VIR_HEIGHT;
    opt.srcX = 0;
    opt.srcY = 0;
    opt.srcCrop8R = (wid_alig16 != SRC_WIDTH) ? 1 : 0;
    opt.srcCrop8D = (hei_alig16 != SRC_HEIGHT) ? 1 : 0;

    // 配置目标参数
    wid_alig16 = ((DST_WIDTH + ALIGN_16_VALUE) & (~BIT_MASK_ALIGN_16));
    hei_alig16 = ((DST_HEIGHT + ALIGN_16_VALUE) & (~BIT_MASK_ALIGN_16));
    opt.dstAddr = dst->phy_addr;
    opt.dstFormat = PP_OUT_FORMAT_YUV420INTERLAVE;
    opt.dstWidth = DST_WIDTH;
    opt.dstHeight = DST_HEIGHT;
    opt.dstHStride = DST_VIR_WIDTH;
    opt.dstVStride = DST_VIR_HEIGHT;
    opt.dstX = 0;
    opt.dstY = 0;
    opt.deinterlace = 0;
    opt.rotation = PP_ROTATION_RIGHT_90;
    opt.vpuFd = vpuFd;

    // 执行PP处理
    ret |= ppOp::ppOpInit(&hnd, &opt);
    ret |= ppOp::ppOpPerform(hnd);
    ret |= ppOp::ppOpSync(hnd);
    ret |= ppOp::ppOpRelease(hnd);
    if (ret) {
        ALOGE("ppOp process failed, ret: %d", ret);
        return ret;
    }
    return 0;
}

static int saveOutputYuv(void *dstVirAddr)
{
    FILE *fpw = fopen("/data/testout.yuv", "wb+");
    if (!fpw) {
        ALOGE("failed to open testout.yuv");
        return -1;
    }
    int ret = fwrite((char *)dstVirAddr, 1,
                     DST_VIR_WIDTH * DST_VIR_HEIGHT * YUV420_TOTAL_SIZE_MULTIPLIER / YUV420_TOTAL_SIZE_DIVISOR, fpw);
    if (ret < 0) {
        ALOGE("fwrite failed ret = %d", ret);
    }
    ret = fclose(fpw);
    if (ret) {
        ALOGE("fclose failed ret = %d", ret);
    }
    return 0;
}

static void releaseAllResources(char *tmpbuf, VPUMemLinear_t *src, VPUMemLinear_t *dst, int vpuFd)
{
    if (tmpbuf) free(tmpbuf);
    if (src->phy_addr) VPUFreeLinear(src);
    if (dst->phy_addr) VPUFreeLinear(dst);
    if (vpuFd > 0) VPUClientRelease(vpuFd);
}

// 主函数
int main()
{
    ALOGI("ppOp test start\n");
    int ret = 0;
    int vpuFd = -1;
    VPUMemLinear_t src = {0};
    VPUMemLinear_t dst = {0};
    char *tmpbuf = nullptr;
    int framecnt = 0;

    // 分配临时缓冲区
    tmpbuf = (char *)malloc(SRC_VIR_WIDTH * SRC_VIR_HEIGHT / YUV420_CHROMA_SIZE_DIVISOR);
    if (tmpbuf == nullptr) {  // 原代码缺失nullptr，补充完整
        ALOGE("Memory allocation failed\n");
        return -1;
    }

    // 初始化VPU资源
    ret = initVpuResources(&src, &dst, &vpuFd);
    if (ret != 0) {
        goto end;
    }

    // 读取YUV文件并转换格式
    ret = readYuvFile(src.vir_addr);
    if (ret == 0) {
        ret = convertPlanarToSemiPlanar(src.vir_addr, tmpbuf);
        VPUMemClean(&src);
    }
    if (ret != 0) {
        goto end;
    }

    // 循环处理帧（仅处理1帧）
    while (1) {
        printf("framecnt=%d\n", framecnt);
        if (framecnt++ >= 1) break;

        // 执行VPU PP处理
        ret = processVpuPP(&src, &dst, vpuFd);
        if (ret != 0) break;

        // 刷新缓存并保存输出
        VPUMemInvalidate(&dst);
        saveOutputYuv(dst.vir_addr);
    }

end:
    // 释放所有资源
    releaseAllResources(tmpbuf, &src, &dst, vpuFd);
    ALOGI("ppOp test end\n");
    return ret;
}
#endif
