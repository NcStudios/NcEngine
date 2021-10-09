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
            void LoadMesh(const std::string& meshPath);

        private:
            void CreateMesh(const std::string& meshPath, std::unordered_map<std::string, Mesh>& accessors, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
            nc::graphics::Graphics* m_graphics;
    };
}