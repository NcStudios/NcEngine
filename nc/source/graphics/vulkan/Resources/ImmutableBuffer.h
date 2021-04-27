#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/Graphics2.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"

#include <vector>
#include <array>
#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    template<typename T>
    struct UsageFlags {};

    template<> 
    struct UsageFlags<Vertex> { static constexpr vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst; };

    template<> 
    struct UsageFlags<uint32_t> { static constexpr vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst; };

    template<class UsageFlag_T>
    concept IncludedUsage = requires
    {
        UsageFlag_T::usage;
    };
    
    template<typename T, IncludedUsage UsageFlag_T = UsageFlags<T>>
    class ImmutableBuffer;
    // Buffer that is intended for infrequent or one-time writes on the CPU, and frequent reads on the GPU.
    template<typename T, IncludedUsage UsageFlag_T>
    class ImmutableBuffer
    {
        public:

            ImmutableBuffer();
            ImmutableBuffer(ImmutableBuffer&&) = delete;
            ImmutableBuffer& operator = (ImmutableBuffer&&) = delete;
            ImmutableBuffer& operator = (const ImmutableBuffer&) = delete;
            ImmutableBuffer(const ImmutableBuffer&) = delete;
            ~ImmutableBuffer();
            
            vk::Buffer* GetBuffer();

            void Bind(const std::vector<T>& data);

        private:

            vulkan::Base* m_base;
            uint32_t m_memoryIndex;
            vk::Buffer m_immutableBuffer;
    };

    template<typename T, IncludedUsage UsageFlag_T>
    ImmutableBuffer<T, UsageFlag_T>::ImmutableBuffer()
    : m_base { d3dresource::GraphicsResourceManager::GetGraphics2()->GetBasePtr() },
      m_memoryIndex { 0 },
      m_immutableBuffer { nullptr }
    {
    }

    template<typename T, IncludedUsage UsageFlag_T>
    ImmutableBuffer<T, UsageFlag_T>::~ImmutableBuffer()
    {
        if (m_immutableBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
        }
    }

    template<typename T, IncludedUsage UsageFlag_T>
    void ImmutableBuffer<T, UsageFlag_T>::Bind(const std::vector<T>& data)
    {
        auto size = static_cast<uint32_t>(sizeof(T) * data.size());

        // Create staging buffer (lives on CPU).
        vk::Buffer stagingBuffer;
        auto stagingBufferMemoryIndex = m_base->CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, true, &stagingBuffer);

        // Map the data onto the staging buffer.
        m_base->MapMemory(stagingBufferMemoryIndex, data, size);

        // Create immutable buffer (lives on GPU).
        m_memoryIndex = m_base->CreateBuffer(size, UsageFlag_T::usage, false, &m_immutableBuffer);

        // Copy staging into immutable buffer.
        Commands::SubmitCopyCommandImmediate(*m_base, stagingBuffer, m_immutableBuffer, size);

        // Destroy the staging buffer.
        m_base->DestroyBuffer(stagingBufferMemoryIndex);
    }

    template<typename T, IncludedUsage UsageFlag_T>
    vk::Buffer* ImmutableBuffer<T, UsageFlag_T>::GetBuffer()
    {
        return &m_immutableBuffer;
    }
}