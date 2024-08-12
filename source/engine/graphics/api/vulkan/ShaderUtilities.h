#pragma once

#include "graphics/api/vulkan/NcVulkan.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include <string>
#include <vector>

namespace nc::graphics::vulkan
{
vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<uint32_t>& code);
std::vector<uint32_t> ReadShader(const std::string& filename);
auto GetStageFlags(shader_stage stage) -> vk::ShaderStageFlags;
} // namespace nc::graphics::vulkan
