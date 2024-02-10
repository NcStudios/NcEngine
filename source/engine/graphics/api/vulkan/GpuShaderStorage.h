#pragma once

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
struct UboUpdateEventData;

namespace vulkan
{
struct PerFrameUboStorage
{
    std::vector<std::string> uniformBufferUids;
    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
};

struct GpuShaderStorage
{
    GpuShaderStorage(GpuAllocator* allocator, ShaderDescriptorSets* descriptorSets, Signal<const UboUpdateEventData&>& onUniformBufferUpdate);
    void UpdateUniformBuffer(const UboUpdateEventData& eventData);

    GpuAllocator* m_allocator;
    ShaderDescriptorSets* m_descriptorSets;
    std::array<PerFrameUboStorage, MaxFramesInFlight> m_perFrameUboStorage;
    nc::Connection<const UboUpdateEventData&> m_onUniformBufferUpdate;
};
} // namespace vulkan
} // namespace nc::graphics
