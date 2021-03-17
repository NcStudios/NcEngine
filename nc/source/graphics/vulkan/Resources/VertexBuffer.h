#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "VulkanResource.h"

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

    class VertexBuffer : public VulkanResource
    {
        public:

            VertexBuffer(std::vector<Vertex> vertices);
            void Bind() noexcept override;
            uint32_t GetId() const;
            const vk::Buffer& GetBuffer() const;
            uint32_t GetSize() const;
            const std::vector<Vertex>& GetVertices() const;

        private:
        
            vulkan::Base* m_base;
            uint32_t m_id;
            uint32_t m_size;
            vk::Buffer m_vertexBuffer;
            uint32_t m_stagingBufferId;
            vk::Buffer m_stagingBuffer;
            std::vector<Vertex> m_vertices;
    };
}