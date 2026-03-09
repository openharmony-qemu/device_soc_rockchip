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

#ifndef OHOS_HDI_NNRT_V2_0_PREPARED_MODEL_VDI_IMPL_H
#define OHOS_HDI_NNRT_V2_0_PREPARED_MODEL_VDI_IMPL_H

#include "rknn_api.h"
#include "ashmem.h"
#include "nnrt/v2_0/iprepared_model.h"

#define TENSOR_DIM_INDEX_0    0   // 张量维度索引0（如批次/Batch）
#define TENSOR_DIM_INDEX_1    1   // 张量维度索引1（如通道/Channel）
#define TENSOR_DIM_INDEX_2    2   // 张量维度索引2（如高度/Height）
#define TENSOR_DIM_INDEX_3    3   // 张量维度索引3（如宽度/Width）

#define RKNN_WANT_FLOAT_ENABLE        1       // RKNN输出启用浮点格式
#define RKNN_WANT_FLOAT_DISABLE       0       // RKNN输出禁用浮点格式
#define RKNN_IS_PREALLOC_DISABLE      0       // RKNN输出禁用预分配缓冲区
#define OUTPUT_LOG_PRINT_LENGTH       32      // 输出数据日志打印的最大长度（字节）
#define FLOAT32_BYTE_SIZE             4       // 32位浮点数据的字节长度
#define MEM_WRITE_OFFSET_ZERO         0       // 内存写入的起始偏移量
#define RKNN_ATTR_INDEX_BASE          0       // RKNN张量属性索引起始值

struct SingleInputParam {
    size_t i;                  // 输入索引
    rknn_tensor_attr* input_attr; // 输入属性
    rknn_input* rknn_input;    // rknn输入结构体
    rknn_context ctx;          // rknn上下文
};

namespace OHOS {
namespace HDI {
namespace Nnrt {
namespace V2_0 {
class PreparedModelService  : public IPreparedModel {
public:
    PreparedModelService ();
    ~PreparedModelService ();

    int32_t ExportModelCache(std::vector<SharedBuffer>& modelCache) override;

    int32_t GetInputDimRanges(std::vector<std::vector<uint32_t>>& minInputDims,
         std::vector<std::vector<uint32_t>>& maxInputDims) override;

    int32_t Run(const std::vector<IOTensor>& inputs, const std::vector<IOTensor>& outputs,
         std::vector<std::vector<int32_t>>& outputDims) override;

    int init(const char* filename);

    int Compile(const void* modelBuffer, size_t length);

    sptr<Ashmem> ParseBuffer(const SharedBuffer& buffer);

private:
    int SetInputs(const std::vector<IOTensor>& inputs);
    int SetOutputs(const std::vector<IOTensor>& outputs);
    int GetMSInputsAndOutputs();
    int UpdateOutput(const std::vector<IOTensor>& outputs,
        std::vector<std::vector<int32_t>>& outputsDims, bool& isOutputBufferEnough);
    void ResetInputAndOutput();
    void ClearInputDims(void);
    void SaveInputDims(const rknn_tensor_attr* attr);
    int GetRknnOutputs(const std::vector<IOTensor>& outputs,
                       rknn_output* rknn_outputs, bool& isOutputBufferEnough);
    int CopyOutputsToSharedMemory(const std::vector<IOTensor>& outputs,
                                  const rknn_output* rknn_outputs);
    int QueryOutputAttributes(std::vector<std::vector<int32_t>>& outputsDims);

private:
    unsigned char* m_model_data {nullptr};
    rknn_context m_ctx {0};
    rknn_input_output_num m_io_num {0};
    std::vector<std::vector<uint32_t>> m_inputDims;
};
} // V2_0
} // Nnrt
} // HDI
} // OHOS

#endif // OHOS_HDI_NNRT_V2_0_PREPARED_MODEL_VDI_IMPL_H