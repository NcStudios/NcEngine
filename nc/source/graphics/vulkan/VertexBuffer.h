#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"

#include <vector>
#include <array>
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan::vertex
{
    vk::VertexInputBindingDescription GetVertexBindingDescription();
    std::array<vk::VertexInputAttributeDescription, 2> GetVertexAttributeDescriptions();

    struct Vertex
    {
        Vector2 Position;
        Vector3 Color;
    };
}

namespace nc::graphics::vulkan
{
    class Base; class Commands;

    class VertexBuffer
    {
        public:
            VertexBuffer(Base& base, Commands& commands, std::vector<vertex::Vertex> vertices);
            void Bind();
            uint32_t GetId() const;
            const vk::Buffer& GetBuffer() const;
            uint32_t GetSize() const;
            const std::vector<vertex::Vertex>& GetVertices() const;

        private:
            vulkan::Base& m_base;
            vulkan::Commands& m_commands;
            uint32_t m_id;
            uint32_t m_size;
            vk::Buffer m_vertexBuffer;
            uint32_t m_stagingBufferId;
            vk::Buffer m_stagingBuffer;
            std::vector<vertex::Vertex> m_vertices;
    };
}