#pragma once

#include "graphics/Mesh.h"

#include <vector>
#include <string>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    vk::VertexInputBindingDescription GetVertexBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 5> GetVertexAttributeDescriptions();

    class MeshManager
    {
        public:
            MeshManager(nc::graphics::Graphics* graphics);
            void LoadMeshes(const std::vector<std::string>& meshPaths);

        private:
            nc::graphics::Graphics* m_graphics;
    };
}