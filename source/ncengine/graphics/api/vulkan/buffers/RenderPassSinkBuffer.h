#pragma once

#include "graphics/api/vulkan/NcVulkan.h"
#include "graphics/shader_resource/ShaderTypes.h"

namespace nc::graphics::vulkan
{
struct RenderPassSinkBuffer
{
    RenderPassSinkBuffer(vk::Device device);

    void Clear() noexcept;

    vk::UniqueSampler sampler;
    std::vector<vk::ImageView> views;
    std::vector<vk::DescriptorImageInfo> imageInfos;
};
} // namespace nc::graphics::vulkan
