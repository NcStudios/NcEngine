#pragma once

#include "resources/ImmutableBuffer.h"

namespace nc
{
struct GpuAccessorChannels;
namespace graphics
{
    class Base; class GpuAllocator;

    struct VertexData
    {
        VertexData(Base* base, GpuAllocator* allocator);

        std::vector<graphics::Vertex> vertices;
        ImmutableBuffer buffer;
    };

    struct IndexData
    {
        IndexData(Base* base, GpuAllocator* allocator);

        std::vector<uint32_t> indices;
        ImmutableBuffer buffer;
    };

    class AssetsStorage
    {
        public:
            AssetsStorage(Base* base, GpuAllocator* allocator, nc::GpuAccessorChannels* gpuAccessorChannels);
            ~AssetsStorage() = default;

            const VertexData& GetVertexData() const noexcept;
            const IndexData& GetIndexData() const noexcept;

            void SetVertexData(const std::vector<graphics::Vertex>& vertices);
            void SetIndexData(const std::vector<uint32_t>& indices);

            void UpdateMeshBuffer(const MeshAsset& meshAsset);

        private:
            Base* m_base;
            nc::GpuAccessorChannels* m_gpuAccessorChannels;
            GpuAllocator* m_allocator;
            VertexData m_vertexData;
            IndexData m_indexData;
            nc::Connection<const nc::MeshAsset&> m_onMeshAddConnection;
    };
}
}