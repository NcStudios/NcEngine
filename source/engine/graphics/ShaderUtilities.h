#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <string>
#include <vector>

namespace nc::graphics
{
    class GpuOptions;

    vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<uint32_t>& code);
    std::vector<uint32_t> ReadShader(const std::string& filename);
}