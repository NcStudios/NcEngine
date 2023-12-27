#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    auto GetVertexBindingDescription() -> vk::VertexInputBindingDescription;
    auto GetVertexAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 7>;
}
