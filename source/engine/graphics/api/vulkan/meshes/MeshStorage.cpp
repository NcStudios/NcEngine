#include "MeshStorage.h"
#include "asset/AssetData.h"

namespace nc::graphics::vulkan
{
MeshStorage::MeshStorage(GpuAllocator* allocator, Signal<const asset::MeshUpdateEventData&>& onMeshUpdate)
    : m_allocator{allocator},
      m_vertexBuffer{},
      m_indexBuffer{},
      m_onMeshUpdateConnection{onMeshUpdate.Connect(this, &MeshStorage::UpdateBuffer)}
{
}

void MeshStorage::UpdateBuffer(const asset::MeshUpdateEventData& eventData)
{
    m_vertexBuffer = MeshBuffer(m_allocator, eventData.vertices);
    m_indexBuffer = MeshBuffer(m_allocator, eventData.indices);
}
} // namespace nc::graphics::vulkan
