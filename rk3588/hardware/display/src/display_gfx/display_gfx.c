/*
 * Copyright (c) 2021-2023 Rockchip Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include "im2d.h"
#include "rga.h"
#include "RgaUtils.h"
#include "display_log.h"
#include "securec.h"
#include "display_gfx.h"

#define ALIGN_UP(x, a) ((((x) + ((a)-1)) / (a)) * (a))
#define MIN_THRESHOLD   2
#define MAX_THRESHOLD   4
#define FORCED_VALUE    2
#define MIN_WIDTH       2
#define MAX_HEIGHT      64
#define RGA_ALIGNMENT 2
#define REMAINDER_ODD 1

typedef struct {
    rga_buffer_t dstRgaBuffer;
    rga_buffer_t srcRgaBuffer;
    rga_buffer_t bRgbBuffer;
    rga_buffer_handle_t src_handle;
    rga_buffer_handle_t dst_handle;
    rga_buffer_handle_t brgb_handle;
    im_rect srect;
    im_rect drect;
    im_rect prect;
    int32_t blendType;
    int32_t transformType;
    int32_t usage;
    uint8_t isYuv;
} BufferContainer;

inline bool IsOdd(int32_t value)
{
    return (value % RGA_ALIGNMENT == REMAINDER_ODD);
}

inline int32_t AlignDownIfOdd(int32_t value)
{
    return IsOdd(value) ? (value - 1) : value;
}

inline int32_t AlignUpIfOdd(int32_t value)
{
    return IsOdd(value) ? (value + 1) : value;
}

int32_t rkInitGfx()
{
    DISPLAY_LOGE("%s\n", querystring(RGA_ALL));
    return DISPLAY_SUCCESS;
}

int32_t rkDeinitGfx()
{
    return DISPLAY_SUCCESS;
}

RgaSURF_FORMAT colorSpaceModeChange(PixelFormat color, uint8_t *isYuv)
{
    RgaSURF_FORMAT rkFormat;
    switch (color) {
        case PIXEL_FMT_RGB_565:          /**< RGB565 format */
            rkFormat = RK_FORMAT_RGB_565;
            *isYuv = 0;
            break;
        case PIXEL_FMT_RGBA_4444:        /**< RGBA4444 format */
            rkFormat = RK_FORMAT_RGBA_4444;
            *isYuv = 0;
            break;
        case PIXEL_FMT_RGBA_5551:        /**< RGBA5551 format */
            rkFormat = RK_FORMAT_RGBA_5551;
            *isYuv = 0;
            break;
        case PIXEL_FMT_RGBX_8888:        /**< RGBX8888 format */
            rkFormat = RK_FORMAT_RGBX_8888;
            *isYuv = 0;
            break;
        case PIXEL_FMT_RGBA_8888:        /**< RGBA8888 format */
            rkFormat = RK_FORMAT_RGBA_8888;
            *isYuv = 0;
            break;
        case PIXEL_FMT_RGB_888:          /**< RGB888 format */
            rkFormat = RK_FORMAT_RGB_888;
            *isYuv = 0;
            break;
        case PIXEL_FMT_BGR_565:          /**< BGR565 format */
            rkFormat = RK_FORMAT_BGR_565;
            *isYuv = 0;
            break;
        case PIXEL_FMT_BGRA_4444:        /**< BGRA4444 format */
            rkFormat = RK_FORMAT_BGRA_4444;
            *isYuv = 0;
            break;
        case PIXEL_FMT_BGRA_5551:        /**< BGRA5551 format */
            rkFormat = RK_FORMAT_BGRA_5551;
            *isYuv = 0;
            break;
        case PIXEL_FMT_BGRX_8888:        /**< BGRX8888 format */
            rkFormat = RK_FORMAT_BGRX_8888;
            *isYuv = 0;
            break;
        case PIXEL_FMT_BGRA_8888:        /**< BGRA8888 format */
            rkFormat = RK_FORMAT_BGRA_8888;
            *isYuv = 0;
            break;
        case PIXEL_FMT_YCBCR_422_SP:     /**< YCBCR422 semi-planar format */
            rkFormat = RK_FORMAT_YCbCr_420_SP;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCRCB_422_SP:     /**< YCRCB422 semi-planar format */
            rkFormat = RK_FORMAT_YCrCb_422_SP;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCBCR_420_SP:     /**< YCBCR420 semi-planar format */
            rkFormat = RK_FORMAT_YCbCr_420_SP;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCRCB_420_SP:     /**< YCRCB420 semi-planar format */
            rkFormat = RK_FORMAT_YCrCb_420_SP;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCBCR_422_P:      /**< YCBCR422 planar format */
            rkFormat = RK_FORMAT_YCbCr_422_P;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCRCB_422_P:      /**< YCRCB422 planar format */
            rkFormat = RK_FORMAT_YCrCb_422_P;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCBCR_420_P:      /**< YCBCR420 planar format */
            rkFormat = RK_FORMAT_YCbCr_420_P;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YCRCB_420_P:      /**< YCRCB420 planar format */
            rkFormat = RK_FORMAT_YCrCb_420_P;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YUYV_422_PKG:     /**< YUYV422 packed format */
            rkFormat = RK_FORMAT_YUYV_422;
            *isYuv = 1;
            break;
        case PIXEL_FMT_UYVY_422_PKG:     /**< UYVY422 packed format */
            rkFormat = RK_FORMAT_UYVY_422;
            *isYuv = 1;
            break;
        case PIXEL_FMT_YVYU_422_PKG:     /**< YVYU422 packed format */
            rkFormat = RK_FORMAT_YUYV_422;
            *isYuv = 1;
            break;
        case PIXEL_FMT_VYUY_422_PKG:     /**< VYUY422 packed format */
            rkFormat = RK_FORMAT_VYUY_422;
            *isYuv = 1;
            break;
        default:
//          PIXEL_FMT_CLUT8:        /**< CLUT8 format */
//          PIXEL_FMT_CLUT1,            /**< CLUT1 format */
//          PIXEL_FMT_CLUT4,            /**< CLUT4 format */
//          PIXEL_FMT_RGBA_5658,        /**< RGBA5658 format */
//          PIXEL_FMT_RGBX_4444,        /**< RGBX4444 format */
//          PIXEL_FMT_RGB_444,          /**< RGB444 format */
//          PIXEL_FMT_RGBX_5551,        /**< RGBX5551 format */
//          PIXEL_FMT_RGB_555,          /**< RGB555 format */
//          PIXEL_FMT_BGRX_4444,        /**< BGRX4444 format */
//          PIXEL_FMT_BGRX_5551,        /**< BGRX5551 format */
//          PIXEL_FMT_YUV_422_I,        /**< YUV422 interleaved format */
            rkFormat = RK_FORMAT_UNKNOWN;
            break;
    }
    return rkFormat;
}

int32_t rkFillRect(ISurface *iSurface, IRect *rect, uint32_t color, GfxOpt *opt)
{
    rga_buffer_t dst;
    rga_buffer_handle_t dst_handle = 0;
    im_handle_param_t dst_param;
    im_rect imRect;
    IM_STATUS ret;
    uint8_t isYuv;
    int32_t ret_val = DISPLAY_SUCCESS;

    errno_t eok = memset_s((void *)&imRect, sizeof(imRect), 0, sizeof(imRect));
    if (eok != EOK) {
        DISPLAY_LOGE("memset_s failed");
        ret_val = false;
        goto EXIT;
    }
    imRect.x = rect->x;
    imRect.y = rect->y;
    imRect.width = rect->w;
    imRect.height = rect->h;

    eok = memset_s((void *)&dst, sizeof(dst), 0, sizeof(dst));
    if (eok != EOK) {
        DISPLAY_LOGE("memset_s failed");
        ret_val = false;
        goto EXIT;
    }

    dst_param.width = (uint32_t)iSurface->width;
    dst_param.height = (uint32_t)iSurface->height;
    dst_param.format = (uint32_t)colorSpaceModeChange(iSurface->enColorFmt, &isYuv);

    dst_handle = importbuffer_fd(iSurface->phyAddr, &dst_param);
    if (dst_handle == 0) {
        DISPLAY_LOGE("import dst_handle error!");
        ret_val = DISPLAY_FAILURE;
        goto EXIT;
    }
    dst = wrapbuffer_handle(dst_handle, iSurface->width, iSurface->height, dst_param.format);

    ret = imfill(dst, imRect, color);
    if (ret != IM_STATUS_SUCCESS) {
        DISPLAY_LOGE("gfx imfill %{public}s", imStrError(ret));
        ret_val = DISPLAY_FAILURE;
    }

EXIT:
    if (dst_handle) {
        releasebuffer_handle(dst_handle);
    }
    return ret_val;
}

int32_t blendTypeChange(BlendType blendType)
{
    int32_t rkBlendType;
    switch (blendType) {
        case BLEND_SRC:              /**< SRC blending */
            rkBlendType = IM_ALPHA_BLEND_SRC;
            break;
        case BLEND_DST:              /**< SRC blending */
            rkBlendType = IM_ALPHA_BLEND_DST;
            break;
        case BLEND_SRCOVER:          /**< SRC_OVER blending */
            rkBlendType = IM_ALPHA_BLEND_SRC_OVER;
            break;
        case BLEND_DSTOVER:          /**< DST_OVER blending */
            rkBlendType = IM_ALPHA_BLEND_DST_OVER;
            break;
        default:
            /* Fix up later */
//        BLEND_NONE         /**< No blending */
//        BLEND_CLEAR:            /**< CLEAR blending */
//        BLEND_SRCIN:            /**< SRC_IN blending */
//        BLEND_DSTIN:            /**< DST_IN blending */
//        BLEND_SRCOUT:           /**< SRC_OUT blending */
//        BLEND_DSTOUT:           /**< DST_OUT blending */
//        BLEND_SRCATOP:          /**< SRC_ATOP blending */
//        BLEND_DSTATOP:          /**< DST_ATOP blending */
//        BLEND_ADD:              /**< ADD blending */
//        BLEND_XOR:              /**< XOR blending */
//        BLEND_DST:              /**< DST blending */
//        BLEND_AKS:              /**< AKS blending */
//        BLEND_AKD:              /**< AKD blending */
//        BLEND_BUTT:              /**< Null operation */
            rkBlendType = IM_STATUS_NOT_SUPPORTED;
            break;
    }
    return rkBlendType;
}

int32_t TransformTypeChange(TransformType type)
{
    int32_t rkTransformType;
    switch (type) {
        case ROTATE_90:            /**< Rotation by 90 degrees */
            rkTransformType = IM_HAL_TRANSFORM_ROT_90;
            break;
        case ROTATE_180:           /**< Rotation by 180 degrees */
            rkTransformType = IM_HAL_TRANSFORM_ROT_180;
            break;
        case ROTATE_270:           /**< Rotation by 270 degrees */
            rkTransformType = IM_HAL_TRANSFORM_ROT_270;
            break;
        case MIRROR_H:             /**< Mirror transform horizontally */
            rkTransformType = IM_HAL_TRANSFORM_FLIP_H;
            break;
        case MIRROR_V:             /**< Mirror transform vertically */
            rkTransformType = IM_HAL_TRANSFORM_FLIP_V;
            break;
        case MIRROR_H_ROTATE_90:   /**< Mirror transform horizontally, rotation by 90 degrees */
            rkTransformType = IM_HAL_TRANSFORM_ROT_90 | IM_HAL_TRANSFORM_FLIP_V;
            break;
        case MIRROR_V_ROTATE_90:   /**< Mirror transform vertically, rotation by 90 degrees */
            rkTransformType = IM_HAL_TRANSFORM_ROT_90 | IM_HAL_TRANSFORM_FLIP_H;
            break;
        default:
            rkTransformType = 0;   /**< No rotation */
            break;
    }
    return rkTransformType;
}
int32_t ImportSourceAndDestinationBuffers(ISurface *dstSurface, ISurface *srcSurface, BufferContainer *buffers)
{
    im_handle_param_t src_param;
    im_handle_param_t dst_param;

    dst_param.width = (uint32_t)dstSurface->width;
    dst_param.height = (uint32_t)dstSurface->height;
    dst_param.format = (uint32_t)colorSpaceModeChange(dstSurface->enColorFmt, &(buffers->isYuv));
    if (buffers->isYuv == 1) {
        DISPLAY_LOGE("rk gfx do not support dst buffer is yuv format");
        return DISPLAY_FAILURE;
    }

    src_param.width = (uint32_t)srcSurface->width;
    src_param.height = (uint32_t)srcSurface->height;
    src_param.format = (uint32_t)colorSpaceModeChange(srcSurface->enColorFmt, &(buffers->isYuv));

    buffers->dst_handle = importbuffer_fd(dstSurface->phyAddr, &dst_param);
    if (buffers->dst_handle == 0) {
        DISPLAY_LOGE("import dst_handle error!");
        return DISPLAY_FAILURE;
    }

    if (srcSurface->phyAddr == 0) {
        DISPLAY_LOGE("source fd is invalid");
        return DISPLAY_FAILURE;
    }

    buffers->src_handle = importbuffer_fd(srcSurface->phyAddr, &src_param);
    if (buffers->src_handle == 0) {
        DISPLAY_LOGE("import src_handle error!");
        return DISPLAY_FAILURE;
    }

    buffers->srcRgaBuffer = wrapbuffer_handle(buffers->src_handle, \
        srcSurface->width, srcSurface->height, src_param.format);
    buffers->dstRgaBuffer = wrapbuffer_handle(buffers->dst_handle, \
        dstSurface->width, dstSurface->height, dst_param.format);

    buffers->srcRgaBuffer.wstride = ALIGN_UP(srcSurface->width, 16);
    buffers->srcRgaBuffer.hstride = ALIGN_UP(srcSurface->height, 2);
    buffers->dstRgaBuffer.wstride = ALIGN_UP(dstSurface->width, 16);
    buffers->dstRgaBuffer.hstride = ALIGN_UP(dstSurface->height, 2);

    return DISPLAY_SUCCESS;
}

void handleSpecialRectCases(BufferContainer *buffers, GfxOpt *opt)
{
    if (buffers->srect.width <= MIN_WIDTH && buffers->srect.height <= MAX_HEIGHT) {
        // When move mouse (pointer window, or other tiny window?) to right edge of the screen,
        // in the mirror screen, the content in srect(srcRgaBuffer) will be copyied to drect(dstRgaBuffer)
        // with scale or rotate (for example, srect [0, 0, 2, 64] --> drect [1916, 68, 4, 96]).
        // And it may cause rga task timeout in kernel, so we do something here to avoid the thread to be blocked.
        if (opt->rotateType == ROTATE_90 ||
            opt->rotateType == ROTATE_270 ||
            opt->rotateType == MIRROR_H_ROTATE_90 ||
            opt->rotateType == MIRROR_V_ROTATE_90) {
            if (buffers->drect.height > MIN_THRESHOLD && buffers->drect.height <= MAX_THRESHOLD) {
                DISPLAY_LOGD("force drect.height to 2");
                buffers->drect.height = FORCED_VALUE;
            }
        } else {
            if (buffers->drect.width > MIN_THRESHOLD && buffers->drect.width <= MAX_THRESHOLD) {
                DISPLAY_LOGD("force drect.width to 2");
                buffers->drect.width = FORCED_VALUE;
            }
        }
    }
}

int32_t prepareRgaBufferRects(IRect *srcRect, IRect *dstRect, BufferContainer *buffers, GfxOpt *opt)
{
    buffers->srect.x = AlignDownIfOdd(srcRect->x);
    buffers->srect.y = AlignDownIfOdd(srcRect->y);
    buffers->drect.x = AlignDownIfOdd(dstRect->x);
    buffers->drect.y = AlignDownIfOdd(dstRect->y);
    buffers->srect.height = AlignUpIfOdd(srcRect->h);
    buffers->srect.width = AlignUpIfOdd(srcRect->w);
    buffers->drect.height = AlignUpIfOdd(dstRect->h);
    buffers->drect.width = AlignUpIfOdd(dstRect->w);

    handleSpecialRectCases(buffers, opt);
    if (opt->blendType) {
        buffers->blendType = blendTypeChange(opt->blendType);
        if (buffers->blendType > 0) {
            buffers->usage |= buffers->blendType;
        } else if (buffers->blendType == IM_STATUS_NOT_SUPPORTED) {
            return DISPLAY_FAILURE;
        }
    }
    if (opt->rotateType) {
        buffers->transformType = TransformTypeChange(opt->rotateType);
        if (buffers->transformType != 0) {
            buffers->usage |= buffers->transformType;
        }
    }
    if (opt->enableScale) {
        DISPLAY_LOGE("gfx scale from (%{puhblic}d, %{public}d) to (%{public}d, %{public}d)", \
            buffers->srcRgaBuffer.width, buffers->srcRgaBuffer.height,
            buffers->dstRgaBuffer.width, buffers->dstRgaBuffer.height);
    }
    buffers->usage |= IM_SYNC;

    return DISPLAY_SUCCESS;
}

int32_t handleSrcOverBlending(ISurface *srcSurface, BufferContainer *buffers, GfxOpt *opt)
{
    errno_t eok;
    buffers->usage = 0;
    if (opt->enableScale == 0) {
        eok = memset_s(&(buffers->srect), sizeof(buffers->srect), 0, sizeof(buffers->srect));
        if (eok != EOK) {
            return DISPLAY_FAILURE;
        }
        buffers->srect.width = buffers->srcRgaBuffer.width;
        buffers->srect.height = buffers->srcRgaBuffer.height;

        eok = memset_s(&(buffers->drect), sizeof(buffers->drect), 0, sizeof(buffers->drect));
        if (eok != EOK) {
            return DISPLAY_FAILURE;
        }
        buffers->drect.x = buffers->dstRgaBuffer.width - buffers->srcRgaBuffer.width;
        buffers->drect.y = buffers->dstRgaBuffer.height - buffers->srcRgaBuffer.height;
        buffers->drect.width = buffers->srcRgaBuffer.width;
        buffers->drect.height = buffers->srcRgaBuffer.height;
    }
    buffers->srcRgaBuffer.wstride = srcSurface->stride;
    buffers->usage = buffers->transformType | IM_SYNC | IM_ALPHA_BLEND_PRE_MUL;
    buffers->srcRgaBuffer.color_space_mode = IM_YUV_TO_RGB_BT601_LIMIT;
    int ret = improcess(buffers->srcRgaBuffer, buffers->dstRgaBuffer, buffers->bRgbBuffer,
                        buffers->srect, buffers->drect, buffers->prect,
                        buffers->usage);
    if (ret != IM_STATUS_SUCCESS) {
        DISPLAY_LOGE("gfx improcess %{public}s", imStrError(ret));
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

int32_t handleDstOverBlending(BufferContainer *buffers)
{
    im_handle_param_t brgb_param;

    brgb_param.width = buffers->dstRgaBuffer.width;
    brgb_param.height = buffers->dstRgaBuffer.height;
    brgb_param.format = RK_FORMAT_RGBA_8888;

    buffers->brgb_handle = importbuffer_fd(-1, &brgb_param);
    if (buffers->brgb_handle == 0) {
        DISPLAY_LOGE("import dst_handle error!");
        return DISPLAY_FAILURE;
    }
    buffers->bRgbBuffer = wrapbuffer_handle(buffers->brgb_handle, buffers->dstRgaBuffer.width,
                                            buffers->dstRgaBuffer.height, RK_FORMAT_RGBA_8888);
    int ret = memcpy_s(&(buffers->prect), sizeof(buffers->drect), &(buffers->drect), sizeof(buffers->drect));
    if (ret != EOK) {
        return DISPLAY_FAILURE;
    }
    ret = improcess(buffers->srcRgaBuffer, buffers->bRgbBuffer, buffers->dstRgaBuffer,
                    buffers->srect, buffers->prect, buffers->drect, buffers->usage);
    if (ret != IM_STATUS_SUCCESS) {
        DISPLAY_LOGE("gfx improcess %{public}s", imStrError(ret));
        return DISPLAY_FAILURE;
    } else {
        ret = imcopy(buffers->bRgbBuffer, buffers->dstRgaBuffer);
        if (ret != IM_STATUS_SUCCESS) {
            DISPLAY_LOGE("gfx imcopy %{public}s", imStrError(ret));
            return DISPLAY_FAILURE;
        }
    }

    return DISPLAY_SUCCESS;
}

int32_t performImageBlending(ISurface *srcSurface, BufferContainer *buffers, GfxOpt *opt)
{
    IM_STATUS ret = 0;
    if (buffers->isYuv == 1) {
        if (buffers->blendType == IM_ALPHA_BLEND_SRC_OVER ||
            buffers->blendType == IM_ALPHA_BLEND_SRC) {
            return handleSrcOverBlending(srcSurface, buffers, opt);
        } else if (buffers->blendType == IM_ALPHA_BLEND_DST_OVER) {
            return handleDstOverBlending(buffers);
        }
    } else {
        ret = improcess(buffers->srcRgaBuffer, buffers->dstRgaBuffer, buffers->bRgbBuffer,
                        buffers->srect, buffers->drect, buffers->prect, buffers->usage);
        if (ret != IM_STATUS_SUCCESS) {
            DISPLAY_LOGE("gfx improcess %{public}s", imStrError(ret));
            return DISPLAY_FAILURE;
        }
    }

    return DISPLAY_SUCCESS;
}

int32_t doFlit(ISurface *srcSurface, IRect *srcRect, ISurface *dstSurface, IRect *dstRect, GfxOpt *opt)
{
    int32_t ret = 0;
    BufferContainer buffers;
    int32_t ret_val = DISPLAY_SUCCESS;

    errno_t eok = memset_s(&buffers, sizeof(buffers), 0, sizeof(buffers));
    if (eok != EOK) {
        DISPLAY_LOGE("memset_s failed");
        ret_val = DISPLAY_FAILURE;
        goto EXIT;
    }

    ret = ImportSourceAndDestinationBuffers(dstSurface, srcSurface, &buffers);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_LOGE("ImportSourceAndDestinationBuffers failed");
        ret_val = DISPLAY_FAILURE;
        goto EXIT;
    }

    ret = prepareRgaBufferRects(srcRect, dstRect, &buffers, opt);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_LOGE("prepareRgaBufferRects failed");
        ret_val = DISPLAY_FAILURE;
        goto EXIT;
    }

    ret = performImageBlending(srcSurface, &buffers, opt);
    if (ret != DISPLAY_SUCCESS) {
        DISPLAY_LOGE("performImageBlending failed");
        ret_val = DISPLAY_FAILURE;
        goto EXIT;
    }

EXIT:
    if (buffers.brgb_handle) {
        releasebuffer_handle(buffers.brgb_handle);
    }
    if (buffers.src_handle) {
        releasebuffer_handle(buffers.src_handle);
    }
    if (buffers.dst_handle) {
        releasebuffer_handle(buffers.dst_handle);
    }
    return ret_val;
}

int32_t rkBlit(ISurface *srcSurface, IRect *srcRect, ISurface *dstSurface, IRect *dstRect, GfxOpt *opt)
{
    CHECK_NULLPOINTER_RETURN_VALUE(srcSurface, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(srcRect, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(dstSurface, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(dstRect, DISPLAY_NULL_PTR);
    CHECK_NULLPOINTER_RETURN_VALUE(opt, DISPLAY_NULL_PTR);

    DISPLAY_LOGD("gfx src fd %{public}d, w %{public}d, h %{publuc}d, sw %{public}d sh %{public}d",
        (int32_t)srcSurface->phyAddr, srcSurface->width, srcSurface->height, ALIGN_UP(srcSurface->width, 16),
        ALIGN_UP(srcSurface->height, 2));
    DISPLAY_LOGD("gfx dst fd %{public}d, w %{public}d, h %{public}d, sw %{public}d sh %{public}d",
        (int32_t)dstSurface->phyAddr, dstSurface->width, dstSurface->height, ALIGN_UP(dstSurface->width, 16),
        ALIGN_UP(dstSurface->height, 2));

    if (doFlit(srcSurface, srcRect, dstSurface, dstRect, opt) < 0)
        return DISPLAY_FAILURE;
    else
        return DISPLAY_SUCCESS;
}

int32_t rkSync(int32_t timeOut)
{
    return DISPLAY_SUCCESS;
}

int32_t GfxInitialize(GfxFuncs **funcs)
{
    DISPLAY_CHK_RETURN((funcs == NULL), DISPLAY_PARAM_ERR, DISPLAY_LOGE("info is null"));
    GfxFuncs *gfxFuncs = (GfxFuncs *)malloc(sizeof(GfxFuncs));
    DISPLAY_CHK_RETURN((gfxFuncs == NULL), DISPLAY_NULL_PTR, DISPLAY_LOGE("gfxFuncs is nullptr"));
    errno_t eok = memset_s((void *)gfxFuncs, sizeof(GfxFuncs), 0, sizeof(GfxFuncs));
    if (eok != EOK) {
        DISPLAY_LOGE("memset_s failed");
        free(gfxFuncs);
        return DISPLAY_FAILURE;
    }
    gfxFuncs->InitGfx = rkInitGfx;
    gfxFuncs->DeinitGfx = rkDeinitGfx;
    gfxFuncs->FillRect = rkFillRect;
    gfxFuncs->Blit = rkBlit;
    gfxFuncs->Sync = rkSync;
    *funcs = gfxFuncs;

    return DISPLAY_SUCCESS;
}

int32_t GfxUninitialize(GfxFuncs *funcs)
{
    CHECK_NULLPOINTER_RETURN_VALUE(funcs, DISPLAY_NULL_PTR);
    free(funcs);
    DISPLAY_LOGD("%s: gfx uninitialize success", __func__);
    return DISPLAY_SUCCESS;
}
