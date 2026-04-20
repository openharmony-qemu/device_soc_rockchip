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

#include "nnrt_device_vdi_impl.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include <securec.h>

#define HDF_LOG_TAG    nnrt_device_service

namespace OHOS {
namespace HDI {
namespace Nnrt {
namespace V2_0 {

NnrtDeviceVdiImpl::~NnrtDeviceVdiImpl()
{
    for (auto ash : m_ashmems) {
        ash.second->UnmapAshmem();
        ash.second->CloseAshmem();
    }
}

int32_t NnrtDeviceVdiImpl::GetDeviceName(std::string& name)
{
    name = "RK3588-NPU";
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::GetVendorName(std::string& name)
{
    name = "Rockchip";
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::GetDeviceType(DeviceType& deviceType)
{
    deviceType = DeviceType::ACCELERATOR;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::GetDeviceStatus(DeviceStatus& status)
{
    status = DeviceStatus::AVAILABLE;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::GetSupportedOperation(const Model& model, std::vector<bool>& ops)
{
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::IsFloat16PrecisionSupported(bool& isSupported)
{
    isSupported = true;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::IsPerformanceModeSupported(bool& isSupported)
{
    isSupported = true;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::IsPrioritySupported(bool& isSupported)
{
    isSupported = true;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::IsDynamicInputSupported(bool& isSupported)
{
    isSupported = true;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::PrepareModel(const Model& model, const ModelConfig& config,
    sptr<OHOS::HDI::Nnrt::V2_0::IPreparedModel>& preparedModel)
{
    HDF_LOGI("NnrtDeviceVdiImpl::PrepareModel enter %{public}s.", model.name.c_str());

    sptr<PreparedModelService> service = new (std::nothrow) PreparedModelService();
    if (service == nullptr) {
        HDF_LOGE("Create new PreparedModelService instance failed.");
        return HDF_FAILURE;
    }
    
    int ret = service->init(model.name.c_str());
    if (ret != 0) {
        HDF_LOGE("Prepared model failed.");
        return ret;
    }

    preparedModel = service;
    
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::IsModelCacheSupported(bool& isSupported)
{
    isSupported = true;
    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::PrepareModelFromModelCache(const std::vector<SharedBuffer>& modelCache,
    const ModelConfig& config, sptr<OHOS::HDI::Nnrt::V2_0::IPreparedModel>& preparedModel)
{
    HDF_LOGI("PrepareModelFromModelCache start");

    printf("PrepareModelFromModelCache start\n");
    if (modelCache.size() != 1) {
        HDF_LOGE("The size of offlineModels vector is not valid, it should be one elememt in that vector.");
        return HDF_FAILURE;
    }

    sptr<PreparedModelService> service = new (std::nothrow) PreparedModelService();
    if (service == nullptr) {
        HDF_LOGE("Create new PreparedModelService instance failed.");
        return HDF_FAILURE;
    }

    SharedBuffer buffer = modelCache[0];
    sptr<Ashmem> ashptr = new (std::nothrow) Ashmem(buffer.fd, buffer.bufferSize);
    if (ashptr == nullptr) {
        printf("create ashmem failed\n");
        HDF_LOGE("Create shared memory failed.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (!ashptr->MapReadAndWriteAshmem()) {
        printf("map read write ashmem failed\n");
        HDF_LOGE("Map buffer fd to address failed.");
        return HDF_ERR_INVALID_PARAM;
    }

    printf("read from ashmem start, modelCache[0].dataSize=%d\n", buffer.dataSize);
    HDF_LOGI("PrepareModelFromModelCache: read from ashmem start, buffer bufferSize=%d dataSize=%d",
        buffer.bufferSize, buffer.dataSize);
    auto data = const_cast<void*>(ashptr->ReadFromAshmem(buffer.dataSize, 0));
    if (data == nullptr) {
        HDF_LOGE("ModelBuffer cannot be nullptr.");
        return HDF_FAILURE;
    }
    int ret = service->Compile(data, buffer.dataSize);
    if (ret != 0) {
        printf("Compile failed\n");
        HDF_LOGE("Prepared model failed.");
        return ret;
    }

    preparedModel = service;

    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::PrepareOfflineModel(const std::vector<SharedBuffer>& offlineModels,
    const ModelConfig& config, sptr<OHOS::HDI::Nnrt::V2_0::IPreparedModel>& preparedModel)
{
    HDF_LOGI("NnrtDeviceVdiImpl::PrepareOfflineModel enter");
    auto ret = PrepareModelFromModelCache(offlineModels, config, preparedModel);
    if (ret != HDF_SUCCESS) {
        printf("PrepareOfflineModel failed\n");
        HDF_LOGE("PrepareOfflineModel: Prepare offline model failed.");
        return ret;
    }

    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::AllocateBuffer(uint32_t length, SharedBuffer& buffer)
{
    HDF_LOGI("NnrtDeviceVdiImpl::AllocateBuffer enter");

    sptr<Ashmem> ashptr = Ashmem::CreateAshmem("allocateBuffer", length);
    if (ashptr == nullptr) {
        HDF_LOGE("Create shared memory failed.");
        return HDF_FAILURE;
    }

    if (!ashptr->MapReadAndWriteAshmem()) {
        HDF_LOGE("Map allocate buffer failed.");
        return HDF_FAILURE;
    }

    buffer.fd = ashptr->GetAshmemFd();
    buffer.bufferSize = ashptr->GetAshmemSize();
    buffer.offset = 0;
    buffer.dataSize = length;

    HDF_LOGI("AllocateBuffer buffersize %d, datasize %d", buffer.bufferSize, buffer.dataSize);

    m_ashmems[buffer.fd] = ashptr;

    return HDF_SUCCESS;
}

int32_t NnrtDeviceVdiImpl::ReleaseBuffer(const SharedBuffer& buffer)
{
    HDF_LOGI("NnrtDeviceVdiImpl::ReleaseBuffer enter");

    if (buffer.fd == INVALID_FD) {
        HDF_LOGE("Invalid buffer fd, it cannot be %{public}d.", INVALID_FD);
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<Ashmem> ashptr = new (std::nothrow) Ashmem(buffer.fd, buffer.bufferSize);
    if (ashptr == nullptr) {
        HDF_LOGE("Create ashmem failed.");
        return HDF_FAILURE;
    }
    ashptr->UnmapAshmem();
    ashptr->CloseAshmem();

    for (auto& ash : m_ashmems) {
        ash.second->UnmapAshmem();
        ash.second->CloseAshmem();
    }
    m_ashmems.clear();

    return HDF_SUCCESS;
}

extern "C" INnrtDeviceVdi *CreateNnrtDeviceVdi()
{
    return new NnrtDeviceVdiImpl();
}

extern "C" void DestroyNnrtDeviceVdi(INnrtDeviceVdi* vdi)
{
    delete vdi;
}


} // V2_0
} // Nnrt
} // HDI
} // OHOS
