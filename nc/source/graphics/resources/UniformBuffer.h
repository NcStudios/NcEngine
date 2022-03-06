#pragma once

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Vertex.h"
#include "graphics/resources/Environment.h"

#include <vector>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    // Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
    class UniformBuffer
    {
        public:
            UniformBuffer();
            UniformBuffer(Graphics* graphics, const EnvironmentData& data);
            ~UniformBuffer() noexcept;
            UniformBuffer(UniformBuffer&&);
            UniformBuffer& operator=(UniformBuffer&&);
            UniformBuffer& operator=(const UniformBuffer&) = delete;
            UniformBuffer(const UniformBuffer&) = delete;
            
            vk::Buffer* GetBuffer();
            void Bind(Graphics* graphics, const EnvironmentData& data);
            void Clear();

        private:
            Base* m_base;
            uint32_t m_memoryIndex;
            vk::Buffer m_uniformBuffer;
    };
}