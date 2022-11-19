#pragma once

#include "graphics/meshes/Vertex.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    auto GetVertexBindingDescription() -> vk::VertexInputBindingDescription;
    auto GetVertexAttributeDescriptions() -> std::array<vk::VertexInputAttributeDescription, 5>;
}