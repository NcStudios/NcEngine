#pragma once

#include "graphics/api/vulkan/NcVulkan.h"

namespace nc::graphics::vulkan
{
    auto GetVertexBindingDescription() -> vk::VertexInputBindingDescription;
    auto GetVertexAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 7>;
} // namespace nc::graphics::vulkan
