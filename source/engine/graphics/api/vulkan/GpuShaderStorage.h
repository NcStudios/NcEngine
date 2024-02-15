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
struct PerFrameUboStorage
{
    std::vector<uint32_t> uniformBufferUids;
    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
};

struct PerFrameSsboStorage
{
    std::vector<uint32_t> storageBufferUids;
    std::vector<std::unique_ptr<StorageBuffer>> storageBuffers;
};

struct TextureArrayBufferStorage
{
    std::vector<uint32_t> textureArrayBufferUids;
    std::vector<std::unique_ptr<TextureArrayBuffer>> textureArrayBuffers;
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

    std::array<PerFrameSsboStorage, MaxFramesInFlight> m_perFrameSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;

    std::array<PerFrameUboStorage, MaxFramesInFlight> m_perFrameUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;

     // Do not need a set of TextureArrayBuffers per frame: textures are shared across frames
    TextureArrayBufferStorage m_textureArrayBufferStorage;
    nc::Connection<const TabUpdateEventData&> m_onTextureArrayBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
