#pragma once

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Vertex.h"
#include "graphics/Environment.h"
#include "graphics/resources/EnvironmentImpl.h"

#include <vector>
#include "vulkan/vk_mem_alloc.hpp"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    class Graphics;

    // Buffer that is intended for frequent writes on the CPU, and frequent reads on the GPU.
    class UniformBuffer
    {
    public:
        UniformBuffer();
        UniformBuffer(nc::graphics::Graphics* graphics, const void* data, uint32_t size);
        ~UniformBuffer() noexcept;
        UniformBuffer(UniformBuffer&&);
        UniformBuffer& operator=(UniformBuffer&&);
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer(const UniformBuffer&) = delete;

        vk::Buffer* GetBuffer();
        void Bind(nc::graphics::Graphics* graphics, const void* data, uint32_t size);
        void Clear();

    private:
        Base* m_base;
        uint32_t m_memoryIndex;
        vk::Buffer m_uniformBuffer;
    };
}