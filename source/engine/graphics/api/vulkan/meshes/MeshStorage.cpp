#include "MeshStorage.h"
#include "asset/AssetData.h"

namespace nc::graphics
{
MeshStorage::MeshStorage(GpuAllocator* allocator, Signal<const MeshBufferData&>& onMeshUpdate)
    : m_allocator{allocator},
      m_vertexBuffer{},
      m_indexBuffer{},
      m_onMeshUpdateConnection{onMeshUpdate.Connect(this, &MeshStorage::UpdateBuffer)}
{
}

void MeshStorage::UpdateBuffer(const MeshBufferData& meshBufferData)
{
    m_vertexBuffer = ImmutableBuffer(m_allocator, meshBufferData.vertices);
    m_indexBuffer = ImmutableBuffer(m_allocator, meshBufferData.indices);
}
} // namespace nc::graphics
