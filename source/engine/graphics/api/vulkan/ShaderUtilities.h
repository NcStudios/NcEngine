#pragma once

#include "graphics/shader_resource/ShaderTypes.h"
#include "graphics/shader_resource/CubeMapArrayBufferHandle.h"
#include "vulkan/vk_mem_alloc.hpp"

#include <string>
#include <vector>

namespace nc::graphics::vulkan
{
vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<uint32_t>& code);
std::vector<uint32_t> ReadShader(const std::string& filename);
auto GetStageFlags(shader_stage stage) -> vk::ShaderStageFlags;
auto GetCubeMapFormat(CubeMapFormat format) -> vk::Format;
auto GetCubeMapUsage(CubeMapUsage usage) -> vk::ImageUsageFlags;
} // namespace nc::graphics::vulkan
