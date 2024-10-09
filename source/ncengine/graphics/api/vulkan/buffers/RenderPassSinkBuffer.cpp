#include "RenderPassSinkBuffer.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
RenderPassSinkBuffer::RenderPassSinkBuffer(vk::Device device)
    : sampler{CreateShadowMapSampler(device)}
{
}

void RenderPassSinkBuffer::Clear() noexcept
{
    views.clear();
    imageInfos.clear();
}
} // namespace nc::graphics::vulkan
