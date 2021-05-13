#pragma once

#include "graphics/vulkan/Mesh.h"

#include <vector>
#include <string>
#include <unordered_map>
#include "vulkan/vulkan.hpp"

namespace nc::graphics
{
    class Graphics2;

    namespace vulkan
    {
        vk::VertexInputBindingDescription GetVertexBindingDescription();
        std::array<vk::VertexInputAttributeDescription, 5> GetVertexAttributeDescriptions();

        class MeshManager
        {
            public:
                MeshManager(nc::graphics::Graphics2* graphics);
                void LoadMeshes(const std::vector<std::string>& meshPaths);

            private:
                nc::graphics::Graphics2* m_graphics;
        };
    }
}