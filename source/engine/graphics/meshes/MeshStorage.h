#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/Meshes.h"

namespace nc::graphics
{
class GpuAllocator;

class MeshStorage
{
    public:
        MeshStorage(GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        void UpdateBuffer(const MeshBufferData& meshBufferData);
        const VertexBuffer& GetVertexData() const noexcept;
        const IndexBuffer& GetIndexData() const noexcept;

    private:
        GpuAllocator* m_allocator;
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        nc::Connection<const MeshBufferData&> m_onMeshUpdateConnection;
};
} // namespace nc::graphics