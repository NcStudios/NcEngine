#include "UniformBuffer.h"

namespace nc::graphics
{
    UniformBuffer::UniformBuffer()
        : m_memoryIndex { 0 },
          m_uniformBuffer { nullptr }
    {
    }

    UniformBuffer::~UniformBuffer() noexcept
    {
        if (m_uniformBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_uniformBuffer = nullptr;
        }

        m_base = nullptr;
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& other)
        : m_base{std::exchange(other.m_base, nullptr)},
          m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
          m_uniformBuffer{std::exchange(other.m_uniformBuffer, nullptr)}
    {
    }

    UniformBuffer& UniformBuffer::operator = (UniformBuffer&& other)
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_uniformBuffer = std::exchange(other.m_uniformBuffer, nullptr);
        return *this;
    }

    vk::Buffer* UniformBuffer::GetBuffer()
    {
        return &m_uniformBuffer;
    }

    void UniformBuffer::Clear()
    {
        if (m_uniformBuffer)
        {
            m_base->DestroyBuffer(m_memoryIndex);
            m_uniformBuffer = nullptr;
        }

        m_base = nullptr;
    }
}