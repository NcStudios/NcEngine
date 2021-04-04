#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"

#include <vector>
#include <array>
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Base;

    vk::VertexInputBindingDescription GetVertexBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 5> GetVertexAttributeDescriptions();

    struct Vertex
    {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
        Vector3 Tangent;
        Vector3 Bitangent;
    };

    class VertexBuffer
    {
        public:

            VertexBuffer(std::vector<Vertex> vertices);
            ~VertexBuffer();
            VertexBuffer(VertexBuffer&&) = default;
            VertexBuffer& operator = (VertexBuffer&&) = default;
            VertexBuffer& operator = (const VertexBuffer&) = delete;
            VertexBuffer(const VertexBuffer&) = delete;

            void Bind();
            const vk::Buffer& GetBuffer() const;

        private:
        
            vulkan::Base* m_base;
            uint32_t m_memoryIndex;
            uint32_t m_size;
            vk::Buffer m_vertexBuffer;
            std::vector<Vertex> m_vertices;
    };
}