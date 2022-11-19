#include "MeshStorage.h"
#include "graphics/GpuAllocator.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
MeshStorage::MeshStorage(GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_allocator{allocator},
      m_vertexBuffer{m_allocator},
      m_indexBuffer{m_allocator},
      m_onMeshUpdateConnection{gpuAccessorSignals.onMeshUpdate->Connect(this, &MeshStorage::UpdateBuffer)}
{
}

VertexBuffer::VertexBuffer(GpuAllocator* allocator)
    : vertices{std::vector<nc::Vertex>{nc::Vertex()}},
      buffer{ImmutableBuffer(allocator, vertices)}
{
}

IndexBuffer::IndexBuffer(GpuAllocator* allocator)
    : indices{std::vector<uint32_t>{0}},
      buffer{ImmutableBuffer(allocator, indices)}
{
}

const VertexBuffer& MeshStorage::GetVertexData() const noexcept
{
    return m_vertexBuffer;
}

const IndexBuffer& MeshStorage::GetIndexData() const noexcept
{
    return m_indexBuffer;
}

void MeshStorage::UpdateBuffer(const MeshBufferData& meshBufferData)	
{
    m_vertexBuffer.buffer.Clear();
    m_vertexBuffer.buffer = ImmutableBuffer(m_allocator, meshBufferData.vertices);

    m_indexBuffer.buffer.Clear();
    m_indexBuffer.buffer = ImmutableBuffer(m_allocator, meshBufferData.indices);
}
} // namespace nc::graphics