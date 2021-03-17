#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "VulkanResource.h"

#include <vector>
#include <array>
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Base; class Commands;

    class IndexBuffer : public VulkanResource
    {
        public:

            IndexBuffer(std::vector<uint32_t> indices);
            void Bind() noexcept override;
            uint32_t GetId() const;
            const vk::Buffer& GetBuffer() const;
            uint32_t GetSize() const;
            const std::vector<uint32_t>& GetIndices() const;

        private:

            vulkan::Base* m_base;
            uint32_t m_id;
            uint32_t m_size;
            vk::Buffer m_indexBuffer;
            uint32_t m_stagingBufferId;
            vk::Buffer m_stagingBuffer;
            std::vector<uint32_t> m_indices;
    };
}