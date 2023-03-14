#include "MeshStorage.h"
#include "asset/AssetData.h"

namespace nc::graphics
{
MeshStorage::MeshStorage(GpuAllocator* allocator, Signal<const MeshUpdateEventData&>& onMeshUpdate)
    : m_allocator{allocator},
      m_vertexBuffer{},
      m_indexBuffer{},
      m_onMeshUpdateConnection{onMeshUpdate.Connect(this, &MeshStorage::UpdateBuffer)}
{
}

void MeshStorage::UpdateBuffer(const MeshUpdateEventData& eventData)
{
    m_vertexBuffer = ImmutableBuffer(m_allocator, eventData.vertices);
    m_indexBuffer = ImmutableBuffer(m_allocator, eventData.indices);
}
} // namespace nc::graphics
