#include "PPImageArrayBuffer.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
PPImageArrayBuffer::PPImageArrayBuffer(vk::Device device)
    : sampler{CreateShadowMapSampler(device)}
{
}
} // namespace nc::graphics::vulkan
