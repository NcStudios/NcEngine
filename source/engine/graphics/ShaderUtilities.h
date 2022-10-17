#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <string>
#include <vector>

namespace nc::graphics
{
    class GpuOptions;

    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, GpuOptions* gpuOptions);
    std::vector<uint32_t> ReadShader(const std::string& filename);
}