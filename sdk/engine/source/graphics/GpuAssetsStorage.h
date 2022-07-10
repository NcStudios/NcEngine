#pragma once

#include "assets/AssetManagers.h"
#include "resources/ImmutableBuffer.h"

namespace nc
{
namespace graphics
{
class Base;
class GpuAllocator;

struct VertexData
{
    VertexData(Base* base, GpuAllocator* allocator);
    std::span<const nc::Vertex> vertices;
    ImmutableBuffer buffer;
};

struct IndexData
{
    IndexData(Base* base, GpuAllocator* allocator);
    std::span<const uint32_t> indices;
    ImmutableBuffer buffer;
};

class GpuAssetsStorage
{
    public:
        GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);

        const VertexData& GetVertexData() const noexcept;
        const IndexData& GetIndexData() const noexcept;

        void SetVertexData(std::span<const nc::Vertex> vertices);
        void SetIndexData(std::span<const uint32_t> indices);

        void UpdateMeshBuffer(const MeshBufferData& meshAsset);

    private:
        Base* m_base;
        GpuAllocator* m_allocator;
        VertexData m_vertexData;
        IndexData m_indexData;
        nc::Connection<const MeshBufferData&> m_onMeshAddConnection;
};
}
}