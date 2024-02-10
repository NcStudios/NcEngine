#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include "graphics/shader_resource/ShaderTypes.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<uint32_t>& code);
    std::vector<uint32_t> ReadShader(const std::string& filename);
    auto GetStageFlags(shader_stage stage) -> vk::ShaderStageFlags;
}