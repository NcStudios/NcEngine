#pragma once

#include "asset/AssetData.h"
#include "graphics/api/vulkan/buffers/StorageBuffer.h"
#include "graphics/api/vulkan/buffers/TextureArrayBuffer.h"
#include "graphics/api/vulkan/buffers/UniformBuffer.h"
#include "graphics/GraphicsConstants.h"
#include "utility/Signal.h"

#include <string>
#include <memory>
#include <vector>

namespace nc::graphics
{
class GpuAllocator;
class ShaderDescriptorSets;
struct SsboUpdateEventData;
struct UboUpdateEventData;
struct TabUpdateEventData;

namespace vulkan
{
struct UniformBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<UniformBuffer>> buffers;
};

struct StorageBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<StorageBuffer>> buffers;
};

struct TextureArrayBufferStorage
{
    std::vector<uint32_t> uids;
    std::vector<std::unique_ptr<TextureArrayBuffer>> buffers;
};

struct GpuShaderStorage
{
    GpuShaderStorage(vk::Device device,
                     GpuAllocator* allocator, 
                     ShaderDescriptorSets* descriptorSets,
                     Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                     Signal<const UboUpdateEventData&>& onUniformBufferUpdate,
                     Signal<const TabUpdateEventData&>& onTextureArrayBufferUpdate);
                     
    void UpdateStorageBuffer(const SsboUpdateEventData& eventData);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);
    void UpdateTextureArrayBuffer(const TabUpdateEventData& eventData);

    vk::Device m_device;
    GpuAllocator* m_allocator;
    ShaderDescriptorSets* m_descriptorSets;

    std::array<StorageBufferStorage, MaxFramesInFlight> m_perFrameSsboStorage;
    StorageBufferStorage m_staticSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;

    std::array<UniformBufferStorage, MaxFramesInFlight> m_perFrameUboStorage;
    UniformBufferStorage m_staticUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;

    std::array<TextureArrayBufferStorage, MaxFramesInFlight> m_perFrameTabStorage;
    TextureArrayBufferStorage m_staticTabStorage;
    nc::Connection<const TabUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
