#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <string>
#include <vector>

namespace nc::graphics
{
    class Base;

    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, Base* base);
    std::vector<uint32_t> ReadShader(const std::string& filename);
}