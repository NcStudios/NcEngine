#pragma once
#include "graphics/shader_resource/ShaderTypes.h"

#include "vulkan/vk_mem_alloc.hpp"

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
