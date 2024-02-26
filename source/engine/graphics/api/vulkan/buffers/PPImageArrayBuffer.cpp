#include "PPImageArrayBuffer.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
PPImageArrayBuffer::PPImageArrayBuffer(vk::Device device)
    : sampler{CreateShadowMapSampler(device)}
{
}

void PPImageArrayBuffer::Clear() noexcept
{
    views.clear();
    imageInfos.clear();
}
} // namespace nc::graphics::vulkan
