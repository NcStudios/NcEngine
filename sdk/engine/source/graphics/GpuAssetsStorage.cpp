#include "graphics/GpuAssetsStorage.h"
#include "graphics/Base.h"
#include "graphics/GpuAllocator.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_base{base},
      m_allocator{allocator},
      m_vertexData{m_base, m_allocator},
      m_indexData{m_base, m_allocator},
      m_onMeshAddConnection{gpuAccessorSignals.onMeshAdd->Connect(this, &GpuAssetsStorage::UpdateMeshBuffer)}
{
}

const VertexData& GpuAssetsStorage::GetVertexData() const noexcept
{
    return m_vertexData;
}

const IndexData& GpuAssetsStorage::GetIndexData() const noexcept
{
    return m_indexData;
}

void GpuAssetsStorage::SetVertexData(std::span<const nc::Vertex> vertices)
{
    m_vertexData.buffer.Clear();
    m_vertexData.buffer = ImmutableBuffer(m_base, m_allocator, vertices);
}

void GpuAssetsStorage::SetIndexData(std::span<const uint32_t> indices)
{
    m_indexData.buffer.Clear();
    m_indexData.buffer = ImmutableBuffer(m_base, m_allocator, indices);
}

void GpuAssetsStorage::UpdateMeshBuffer(const MeshBufferData& meshBufferData)	
{
    SetVertexData(meshBufferData.vertices);
    SetIndexData(meshBufferData.indices);
}

VertexData::VertexData(Base* base, GpuAllocator* allocator)
    : vertices{std::vector<nc::Vertex>{nc::Vertex()}},
      buffer{ImmutableBuffer(base, allocator, vertices)}
{
}

IndexData::IndexData(Base* base, GpuAllocator* allocator)
    : indices{std::vector<uint32_t>{0}},
      buffer{ImmutableBuffer(base, allocator, indices)}
{
}
}