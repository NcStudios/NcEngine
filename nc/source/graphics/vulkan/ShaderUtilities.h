#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc::graphics
{
    namespace vulkan { class Base; }
}

namespace nc::graphics::vulkan
{
    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, vulkan::Base* base);
    std::vector<uint32_t> ReadShader(const std::string& filename);
}