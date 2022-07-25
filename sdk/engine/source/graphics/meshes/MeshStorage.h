#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/Meshes.h"

namespace nc::graphics
{
class MeshStorage
{
    public:
        MeshStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        void UpdateBuffer(const MeshBufferData& meshBufferData);
        const VertexBuffer& GetVertexData() const noexcept;
        const IndexBuffer& GetIndexData() const noexcept;

    private:
        Base* m_base;
        GpuAllocator* m_allocator;
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        nc::Connection<const MeshBufferData&> m_onMeshUpdateConnection;
};
}