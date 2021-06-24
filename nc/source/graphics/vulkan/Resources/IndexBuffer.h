#pragma once

#include "math/Vector.h"

#include <vector>
#include <array>
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    class Base;

    class IndexBuffer
    {
        public:

            IndexBuffer(std::vector<uint32_t> indices);
            IndexBuffer(IndexBuffer&&) = default;
            IndexBuffer& operator = (IndexBuffer&&) = default;
            IndexBuffer& operator = (const IndexBuffer&) = delete;
            IndexBuffer(const IndexBuffer&) = delete;
            ~IndexBuffer();
            
            const vk::Buffer& GetBuffer() const;
            void Bind();
            const std::vector<uint32_t>& GetIndices() const;

        private:

            vulkan::Base* m_base;
            uint32_t m_memoryIndex;
            uint32_t m_size;
            vk::Buffer m_indexBuffer;
            std::vector<uint32_t> m_indices;
    };
}