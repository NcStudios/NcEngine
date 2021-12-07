#pragma once

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Vertex.h"

#include <vector>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    // Buffer that is intended for infrequent or one-time writes on the CPU, and frequent reads on the GPU.
    class ImmutableBuffer
    {
        public:
            ImmutableBuffer();
            ImmutableBuffer(nc::graphics::Graphics* graphics, const std::vector<uint32_t>& data);
            ImmutableBuffer(nc::graphics::Graphics* graphics, const std::vector<Vertex>& data);
            ~ImmutableBuffer() noexcept;
            ImmutableBuffer(ImmutableBuffer&&);
            ImmutableBuffer& operator=(ImmutableBuffer&&);
            ImmutableBuffer& operator=(const ImmutableBuffer&) = delete;
            ImmutableBuffer(const ImmutableBuffer&) = delete;
            
            vk::Buffer* GetBuffer();
            void Bind(nc::graphics::Graphics* graphics, const std::vector<uint32_t>& data);
            void Bind(nc::graphics::Graphics* graphics, const std::vector<Vertex>& data);
            void Clear();

        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            vk::Buffer m_immutableBuffer;
    };
}