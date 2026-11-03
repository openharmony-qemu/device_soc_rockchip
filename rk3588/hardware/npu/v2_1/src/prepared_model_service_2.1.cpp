/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <iostream>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>
#include "prepared_model_service_2.1.h"
#include <message_parcel.h>
#include <securec.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "rknn_api.h"

using namespace std;

namespace OHOS {
namespace HDI {
namespace Nnrt {
namespace V2_1 {

constexpr uint32_t MIN_DIM = 1;
constexpr uint32_t MAX_DIM = 100000;

PreparedModelService::PreparedModelService()
{
    HDF_LOGI("%{public}s: enter", __func__);
}

PreparedModelService::~PreparedModelService()
{
    HDF_LOGI("%{public}s: enter", __func__);
    if (m_ctx > 0) {
        rknn_destroy(m_ctx);
    }

    if (m_model_data) {
        free(m_model_data);
    }
}

static unsigned char* LoadModel(const char* filename, int* modelSize)
{
    HDF_LOGI("%{public}s: enter", __func__);
    int ret = 0;
    FILE*          fp;
    unsigned char* data = nullptr;
    HDF_LOGI("LoadModel %{public}s", filename);

    fp = fopen(filename, "rb");
    if (fp == nullptr) {
        HDF_LOGE("LoadModel: Open file %{public}s failed", filename);
        return nullptr;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        HDF_LOGE("LoadModel: seek end failed");
        return nullptr;
    }
    long size = ftell(fp);
    if (size <= 0 || size > std::numeric_limits<int>::max()) {
        fclose(fp);
        HDF_LOGE("LoadModel: invalid file size %ld", size);
        return nullptr;
    }
    HDF_LOGI("LoadModel: file size %ld", size);

    ret = fseek(fp, 0, SEEK_SET);
    if (ret != 0) {
        fclose(fp);
        HDF_LOGE("LoadModel: blob seek failure");
        return nullptr;
    }
    data = (unsigned char*)malloc(size);
    if (data == nullptr) {
        fclose(fp);
        HDF_LOGE("LoadModel: buffer malloc failure");
        return nullptr;
    }
    ret = fread(data, 1, size, fp);

    if (fclose(fp) != 0) {
        HDF_LOGE("LoadModel: close file failed");
    }

    *modelSize = static_cast<int>(size);
    return data;
}

static void dump_tensor_attr(rknn_tensor_attr *attr)
{
    HDF_LOGI("dump_tensor_attr: index=%d, name=%{public}s, n_dims=%d, dims=[%d, %d, %d, %d],\
        n_elems=%d, size=%d, fmt=%{public}s, type=%{public}s, qnt_type=%{public}s, zp=%d, scale=%f",
             attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
             attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
             get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

DataType get_data_type(rknn_tensor_type rknn_data_type)
{
    HDF_LOGI("get_data_type enter %d", rknn_data_type);

    DataType data_type = DATA_TYPE_UNKNOWN;
    switch (rknn_data_type) {
        case RKNN_TENSOR_BOOL:
            data_type = DATA_TYPE_BOOL;
            break;
        case RKNN_TENSOR_INT8:
            data_type = DATA_TYPE_INT8;
            break;
        case RKNN_TENSOR_INT16:
            data_type = DATA_TYPE_INT16;
            break;
        case RKNN_TENSOR_INT32:
            data_type = DATA_TYPE_INT32;
            break;
        case RKNN_TENSOR_INT64:
            data_type = DATA_TYPE_INT64;
            break;
        case RKNN_TENSOR_UINT8:
            data_type = DATA_TYPE_UINT8;
            break;
        case RKNN_TENSOR_UINT16:
            data_type = DATA_TYPE_UINT16;
            break;
        case RKNN_TENSOR_UINT32:
            data_type = DATA_TYPE_UINT32;
            break;
        case RKNN_TENSOR_FLOAT16:
            data_type = DATA_TYPE_FLOAT16;
            break;
        case RKNN_TENSOR_FLOAT32:
            data_type = DATA_TYPE_FLOAT32;
            break;
        case RKNN_TENSOR_TYPE_MAX:
        default:
            break;
    }

    return data_type;
}

rknn_tensor_type get_rknn_tensor_type(DataType dataType)
{
    HDF_LOGI("get_rknn_tensor_type enter %d", dataType);

    rknn_tensor_type rknn_data_type = RKNN_TENSOR_TYPE_MAX;
    switch (dataType) {
        case DATA_TYPE_UNKNOWN:
            break;
        case DATA_TYPE_BOOL:
            rknn_data_type = RKNN_TENSOR_BOOL;
            break;
        case DATA_TYPE_INT8:
            rknn_data_type = RKNN_TENSOR_UINT8;
            break;
        case DATA_TYPE_INT16:
            rknn_data_type = RKNN_TENSOR_INT16;
            break;
        case DATA_TYPE_INT32:
            rknn_data_type = RKNN_TENSOR_INT32;
            break;
        case DATA_TYPE_INT64:
            rknn_data_type = RKNN_TENSOR_INT64;
            break;
        case DATA_TYPE_UINT8:
            rknn_data_type = RKNN_TENSOR_UINT8;
            break;
        case DATA_TYPE_UINT16:
            rknn_data_type = RKNN_TENSOR_UINT16;
            break;
        case DATA_TYPE_UINT32:
            rknn_data_type = RKNN_TENSOR_UINT32;
            break;
        case DATA_TYPE_UINT64:
            break;
        case DATA_TYPE_FLOAT16:
            rknn_data_type = RKNN_TENSOR_FLOAT16;
            break;
        case DATA_TYPE_FLOAT32:
            rknn_data_type = RKNN_TENSOR_FLOAT32;
            break;
        case DATA_TYPE_FLOAT64:
            break;
    }

    return rknn_data_type;
}

rknn_tensor_format get_rknn_fmt(Format format)
{
    HDF_LOGI("get_rknn_fmt enter %d", format);

    if (format == FORMAT_NCHW) {
        return RKNN_TENSOR_NCHW;
    } else if (format == FORMAT_NHWC) {
        return RKNN_TENSOR_NHWC;
    }

    return RKNN_TENSOR_UNDEFINED;
}

int32_t PreparedModelService::ExportModelCache(std::vector<SharedBuffer> &modelCache)
{
    HDF_LOGI("ExportModelCache enter");
    return HDF_SUCCESS;
}

int32_t PreparedModelService::GetInputDimRanges(std::vector<std::vector<uint32_t>> &minInputDims,
                                                std::vector<std::vector<uint32_t>> &maxInputDims)
{
    HDF_LOGI("PreparedModelService::GetInputDimRanges enter");
    if (m_inputDims.empty()) {
        HDF_LOGE("GetInputDimRanges: Model has not been prepared yet.");
        return HDF_ERR_INVALID_PARAM;
    }

    minInputDims.clear();
    maxInputDims.clear();

    for (auto inputShape : m_inputDims) {
        std::vector<uint32_t> minInputShape;
        std::vector<uint32_t> maxInputShape;
        for (auto dim : inputShape) {
            minInputShape.push_back(MIN_DIM);
            maxInputShape.push_back(MAX_DIM);
            HDF_LOGI("GetInputDimRanges: dim %u mindim %u, maxdim %u", dim, MIN_DIM, MAX_DIM);
        }
        minInputDims.push_back(std::move(minInputShape));
        maxInputDims.push_back(std::move(maxInputShape));
    }

    return HDF_SUCCESS;
}

int32_t PreparedModelService::Run(const std::vector<IOTensor> &inputs, const std::vector<IOTensor> &outputs,
                                  std::vector<std::vector<int32_t>> &outputDims)
{
    HDF_LOGI("Run: 1 SetInputs");

    auto ret = SetInputs(inputs);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Run: Inputs tensor is invalid.");
        return ret;
    }

    HDF_LOGI("Run 2 rknn_run");
    ret = rknn_run(m_ctx, nullptr);
    if (ret != 0) {
        HDF_LOGE("Run: run RKNN model failed!");
        return HDF_FAILURE;
    }

    HDF_LOGI("Run 2 UpdateOutput");
    bool isOutputBufferEnough{false};
    ret = UpdateOutput(outputs, outputDims, isOutputBufferEnough);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("Run: Update output dimension or data failed.");
        return ret;
    }

    if (!isOutputBufferEnough) {
        HDF_LOGE("Run: Output buffer is not enough.");
        return HDF_ERR_OUT_OF_RANGE;
    }

    rknn_perf_run perf_run;
    ret = rknn_query(m_ctx, RKNN_QUERY_PERF_RUN, &perf_run, sizeof(perf_run));
    HDF_LOGI("Run success,cost %ld us", perf_run.run_duration);

    return HDF_SUCCESS;
}

static int WriteOutputsToAshmem(const std::vector<IOTensor> &outputs, rknn_output rknnOutputs[],
                                uint32_t outputNum, bool &isOutputBufferEnough)
{
    isOutputBufferEnough = true;
    for (size_t i = 0; i < outputNum; i++) {
        auto &output = outputs[i];
        auto dataSize = rknnOutputs[i].size;
        if (dataSize > output.data.bufferSize) {
            HDF_LOGI("Output buffer is not enough. actual size %u, buffer size %{public}u", dataSize,
                     output.data.bufferSize);
            isOutputBufferEnough = false;
        }

        if (isOutputBufferEnough) {
            SharedBuffer buffer = output.data;
            sptr<Ashmem> ashptr = new (std::nothrow) Ashmem(buffer.fd, buffer.bufferSize);
            if (ashptr == nullptr) {
                HDF_LOGE("UpdateOutput: Create shared memory failed.");
                return HDF_ERR_INVALID_PARAM;
            }
            if (!ashptr->MapReadAndWriteAshmem()) {
                HDF_LOGE("UpdateOutput: Map buffer fd to address failed.");
                return HDF_ERR_INVALID_PARAM;
            }
            if (!ashptr->WriteToAshmem(rknnOutputs[i].buf, dataSize, 0)) {
                HDF_LOGE("UpdateOutput: write ro ashmem failed.");
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }
    return HDF_SUCCESS;
}

static int CollectOutputDims(rknn_context ctx, uint32_t outputNum, std::vector<std::vector<int32_t>> &outputsDims)
{
    rknn_tensor_attr output_attrs[outputNum];
    memset_s(output_attrs, sizeof(output_attrs), 0, sizeof(output_attrs));
    for (size_t i = 0; i < outputNum; i++) {
        output_attrs[i].index = i;
        int ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret < 0) {
            HDF_LOGE("UpdateOutput rknn query output attr error i=%zu, ret=%x", i, ret);
            return ret;
        }

        dump_tensor_attr(&(output_attrs[i]));
        std::vector<int32_t> dim;
        for (uint32_t j = 0; j < output_attrs[i].n_dims; ++j) {
            dim.push_back(output_attrs[i].dims[j]);
        }
        outputsDims.push_back(dim);
    }
    return HDF_SUCCESS;
}

int PreparedModelService::UpdateOutput(const std::vector<IOTensor> &outputs,
                                       std::vector<std::vector<int32_t>> &outputsDims, bool &isOutputBufferEnough)
{
    HDF_LOGI("%{public}s: enter", __func__);

    if (outputs.size() != m_io_num.n_output) {
        HDF_LOGE("outputs size is invalid. expect: \
            %u, actual: %{public}zu",
                 m_io_num.n_output, outputs.size());
        return HDF_ERR_INVALID_PARAM;
    }

    rknn_output rknn_outputs[m_io_num.n_output];
    memset_s(rknn_outputs, sizeof(rknn_outputs), 0, sizeof(rknn_outputs));
    for (size_t i = 0; i < m_io_num.n_output; i++) {
        if (outputs[i].dataType == DATA_TYPE_FLOAT32 || outputs[i].dataType == DATA_TYPE_FLOAT16) {
            rknn_outputs[i].want_float = 1;
        }
        rknn_outputs[i].is_prealloc = 0;
        HDF_LOGI("outputs[%zu].dataType=%d,rknn_outputs[%zu].want_float=%d", i, outputs[i].dataType, i,
                 rknn_outputs[i].want_float);
    }
    rknn_outputs_get(m_ctx, m_io_num.n_output, rknn_outputs, NULL);

    int ret = WriteOutputsToAshmem(outputs, rknn_outputs, m_io_num.n_output, isOutputBufferEnough);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    rknn_outputs_release(m_ctx, m_io_num.n_output, rknn_outputs);

    return CollectOutputDims(m_ctx, m_io_num.n_output, outputsDims);
}

int PreparedModelService::SetInputs(const std::vector<IOTensor> &inputs)
{
    HDF_LOGI("%{public}s: enter", __func__);

    sptr<Ashmem> ashptr[m_io_num.n_input];
    rknn_input rknn_inputs[m_io_num.n_input];
    rknn_tensor_attr input_attrs[m_io_num.n_input];
    memset_s(input_attrs, sizeof(input_attrs), 0, sizeof(input_attrs));
    memset_s(rknn_inputs, sizeof(rknn_inputs), 0, sizeof(rknn_inputs));
    for (size_t i = 0; i < m_io_num.n_input; i++) {
        auto &input = inputs[i];
        SharedBuffer buffer = input.data;
        ashptr[i] = new (std::nothrow) Ashmem(buffer.fd, buffer.bufferSize);
        if (ashptr[i] == nullptr) {
            HDF_LOGE("SetInputs: Parse %zuth input data failed.", i);
            return HDF_ERR_INVALID_PARAM;
        }
        if (!ashptr[i]->MapReadAndWriteAshmem()) {
            HDF_LOGE("SetInputs: Map ashmem failed i=%zu.", i);
            return HDF_ERR_INVALID_PARAM;
        }

        auto data = const_cast<void *>(ashptr[i]->ReadFromAshmem(input.data.dataSize, 0));
        int fd = open("/data/_input.rgb", O_CREAT | O_WRONLY);
        write(fd, data, input.data.dataSize);
        close(fd);

        input_attrs[i].index = i;
        if (rknn_query(m_ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr))) {
            HDF_LOGE("rknn_query input attr error i=%zu", i);
            return HDF_ERR_INVALID_PARAM;
        }

        rknn_inputs[i].index = i;
        rknn_inputs[i].size = input.data.dataSize;
        rknn_inputs[i].buf = data;
        rknn_inputs[i].type = get_rknn_tensor_type(input.dataType);
        rknn_inputs[i].fmt = get_rknn_fmt(input.format);
        if (input_attrs[i].type == rknn_inputs[i].type && input_attrs[i].size == rknn_inputs[i].size &&
            input_attrs[i].fmt == rknn_inputs[i].fmt) {
            rknn_inputs[i].pass_through = 1;
        }

        HDF_LOGI("model SetInputs: rknn input index=%u size %d, type %d, format %d "
            "(0-NCHW,1-NHWC),input_attrs[i].type=%d",
            rknn_inputs[i].index, rknn_inputs[i].size, rknn_inputs[i].type, rknn_inputs[i].fmt, input_attrs[i].type);
    }

    int ret = rknn_inputs_set(m_ctx, m_io_num.n_input, rknn_inputs);
    if (ret < 0) {
        HDF_LOGE("rknn_input_set fail! ret=%d", ret);
        return ret;
    }

    return HDF_SUCCESS;
}

int PreparedModelService::SetOutputs(const std::vector<IOTensor> &outputs)
{
    HDF_LOGI("PreparedModelService::SetOutputs enter");
    return HDF_SUCCESS;
}

int PreparedModelService::GetMSInputsAndOutputs(void)
{
    HDF_LOGI("PreparedModelService::GetMSInputsAndOutputs enter");
    return HDF_SUCCESS;
}

void PreparedModelService::ResetInputAndOutput(void)
{
    HDF_LOGI("PreparedModelService::ResetInputAndOutput enter");
    return;
}

sptr<Ashmem> PreparedModelService::ParseBuffer(const SharedBuffer &buffer)
{
    HDF_LOGI("PreparedModelService::ParseBuffer enter");

    if (buffer.fd == -1) {
        HDF_LOGE("ParseBuffer: Invalid buffer fd, it cannot be -1.");
        return nullptr;
    }

    sptr<Ashmem> ashptr = new (std::nothrow) Ashmem(buffer.fd, buffer.bufferSize);
    if (ashptr == nullptr) {
        HDF_LOGE("ParseBuffer: Create shared memory failed.");
        return nullptr;
    }

    if (!ashptr->MapReadAndWriteAshmem()) {
        HDF_LOGE("ParseBuffer: Map buffer fd to address failed.");
        return nullptr;
    }

    const void *data = ashptr->ReadFromAshmem(buffer.dataSize, buffer.offset);
    if (data == nullptr) {
        HDF_LOGE("ParseBuffer: Get data address failed.");
        ashptr->UnmapAshmem();
        ashptr->CloseAshmem();
        return nullptr;
    }

    return ashptr;
}

void PreparedModelService::ClearInputDims(void)
{
    HDF_LOGI("PreparedModelService::ClearInputDims");
    m_inputDims.clear();
    return;
}

void PreparedModelService::SaveInputDims(const rknn_tensor_attr *attr)
{
    HDF_LOGI("%{public}s: enter", __func__);
    vector<uint32_t> shape;
    for (size_t i = 0; i < attr->n_dims; i++) {
        shape.push_back(attr->dims[i]);
    }

    m_inputDims.push_back(std::move(shape));
    return;
}

int PreparedModelService::Compile(const void *modelBuffer, size_t length)
{
    HDF_LOGI("PreparedModelService::Compile enter");

    if (modelBuffer == nullptr || length == 0) {
        HDF_LOGE("ModelBuffer cannot be nullptr and length cannot be zero.");
        return HDF_FAILURE;
    }
    int fd = open("/data/_model.rknn", O_CREAT | O_WRONLY);
    write(fd, modelBuffer, length);
    close(fd);

    int ret = rknn_init(&m_ctx, (void *)modelBuffer, length, RKNN_FLAG_COLLECT_PERF_MASK, NULL);
    if (ret < 0) {
        HDF_LOGE("rknn_init fail! ret=%d", ret);
        return ret;
    }

    rknn_sdk_version version;
    ret = rknn_query(m_ctx, RKNN_QUERY_SDK_VERSION, &version, sizeof(rknn_sdk_version));
    if (ret < 0) {
        HDF_LOGE("rknn_query sdk version error ret=%d", ret);
        return ret;
    }
    HDF_LOGI("PreparedModelService::Compile rknn_query api ver: %{public}s", version.api_version);
    HDF_LOGI("PreparedModelService::Compile rknn_query drv ver: %{public}s", version.drv_version);

    /* Get Model Input Output Info */
    ret = rknn_query(m_ctx, RKNN_QUERY_IN_OUT_NUM, &m_io_num, sizeof(m_io_num));
    if (ret != RKNN_SUCC) {
        HDF_LOGE("rknn_query input-output info fail! ret=%d", ret);
        return ret;
    }

    ClearInputDims();
    rknn_tensor_attr input_attrs[m_io_num.n_input];
    memset_s(input_attrs, sizeof(input_attrs), 0, sizeof(input_attrs));
    for (size_t i = 0; i < m_io_num.n_input; i++) {
        input_attrs[i].index = i;
        ret = rknn_query(m_ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret < 0) {
            HDF_LOGE("rknn_query input attr error i=%zu, ret=%d", i, ret);
            return ret;
        }
        dump_tensor_attr(&(input_attrs[i]));
        SaveInputDims(&(input_attrs[i]));
    }

    return HDF_SUCCESS;
}

int PreparedModelService::init(const char *filename)
{
    HDF_LOGI("PreparedModelService::init enter");

    int model_data_size{0};
    m_model_data = LoadModel(filename, &model_data_size);
    if (m_model_data == nullptr) {
        HDF_LOGE("init: LoadModel fail!");
        return HDF_FAILURE;
    }

    int ret = Compile(m_model_data, model_data_size);
    if (ret < 0) {
        HDF_LOGE("init: compile model failed.");
        return ret;
    }

    return HDF_SUCCESS;
}

}  // V2_1
}  // Nnrt
}  // HDI
}  // OHOS
