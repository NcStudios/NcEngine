#pragma once

#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/resources/ImmutableBuffer.h"

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
                Mesh GetMesh(const std::string& uid) const;
                void Clear();
                vk::Buffer* GetVertexBuffer();
                vk::Buffer* GetIndexBuffer();
                void Bind();


            private:
                bool MeshExists(const std::string& uid) const;
                
                std::unique_ptr<ImmutableBuffer<Vertex>> m_vertexBuffer;
                std::unique_ptr<ImmutableBuffer<uint32_t>> m_indexBuffer;
                std::vector<Vertex> m_vertices;
                std::vector<uint32_t> m_indices;
                std::unordered_map<std::string, Mesh> m_meshes;
        };
    }
}