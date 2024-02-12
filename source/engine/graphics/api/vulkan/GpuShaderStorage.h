#pragma once

#include "graphics/api/vulkan/buffers/UniformBuffer.h"
#include "graphics/api/vulkan/buffers/StorageBuffer.h"
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

struct GpuShaderStorage
{
    GpuShaderStorage(GpuAllocator* allocator, 
                    ShaderDescriptorSets* descriptorSets,
                    Signal<const SsboUpdateEventData&>& onStorageBufferUpdate,
                    Signal<const UboUpdateEventData&>& onUniformBufferUpdate);
    void UpdateStorageBuffer(const SsboUpdateEventData& eventData);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);

    GpuAllocator* m_allocator;
    ShaderDescriptorSets* m_descriptorSets;
    std::array<PerFrameSsboStorage, MaxFramesInFlight> m_perFrameSsboStorage;
    nc::Connection<const SsboUpdateEventData&> m_onStorageBufferUpdate;
    std::array<PerFrameUboStorage, MaxFramesInFlight> m_perFrameUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
