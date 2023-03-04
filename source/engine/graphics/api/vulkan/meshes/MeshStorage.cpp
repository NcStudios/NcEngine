#include "MeshStorage.h"
#include "assets/AssetData.h"
#include "assets/AssetManagers.h"

namespace nc::graphics
{
MeshStorage::MeshStorage(GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : m_allocator{allocator},
      m_vertexBuffer{},
      m_indexBuffer{},
      m_onMeshUpdateConnection{gpuAccessorSignals.onMeshUpdate->Connect(this, &MeshStorage::UpdateBuffer)}
{
}

void MeshStorage::UpdateBuffer(const MeshBufferData& meshBufferData)
{
    m_vertexBuffer = ImmutableBuffer(m_allocator, meshBufferData.vertices);
    m_indexBuffer = ImmutableBuffer(m_allocator, meshBufferData.indices);
}
} // namespace nc::graphics
