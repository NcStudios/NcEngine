#include "graphics/AssetsStorage.h"
#include "graphics/Base.h"
#include "graphics/GpuAllocator.h"

namespace nc::graphics
{   
    AssetsStorage::AssetsStorage(Base* base, GpuAllocator* allocator)
        : m_base{base},
          m_allocator{allocator},
          m_vertexData{m_base, m_allocator},
          m_indexData{m_base, m_allocator}
    {
    }

    const VertexData& AssetsStorage::GetVertexData() const noexcept
    {
        return m_vertexData;
    }

    const IndexData& AssetsStorage::GetIndexData() const noexcept
    {
        return m_indexData;
    }
    
    void AssetsStorage::SetVertexData(const std::vector<graphics::Vertex>& vertices)
    {
        m_vertexData.buffer.Clear();
        m_vertexData.buffer = ImmutableBuffer(m_base, m_allocator, vertices);
    }

    void AssetsStorage::SetIndexData(const std::vector<uint32_t>& indices)
    {
        m_indexData.buffer.Clear();
        m_indexData.buffer = ImmutableBuffer(m_base, m_allocator, indices);
    }

    VertexData::VertexData(Base* base, GpuAllocator* allocator)
    {
        auto vertex = Vertex
        {
            .Position = Vector3{0,0,0},
            .Normal = Vector3{0,0,0},
            .UV = Vector2{0,0},
            .Tangent = Vector3{0,0,0},
            .Bitangent = Vector3{0,0,0}
        };
        vertices = std::vector<Vertex>{vertex};
        buffer = ImmutableBuffer(base, allocator, vertices);
    }

    IndexData::IndexData(Base* base, GpuAllocator* allocator)
    {
        indices = std::vector<uint32_t>{0};
        buffer = ImmutableBuffer(base, allocator, indices);
    }
}