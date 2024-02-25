#pragma once
#include "graphics/shader_resource/ShaderTypes.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics::vulkan
{
struct PPImageArrayBuffer
{
    PPImageArrayBuffer(vk::Device device);

    vk::UniqueSampler sampler;
    std::vector<vk::ImageView> views;
    std::vector<vk::DescriptorImageInfo> imageInfos;
};
} // namespace nc::graphics::vulkan
