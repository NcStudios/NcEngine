#pragma once

#include "resources/ImmutableBuffer.h"

namespace nc::graphics
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
            AssetsStorage(Base* base, GpuAllocator* allocator);
            ~AssetsStorage() = default;

            const VertexData& GetVertexData() const noexcept;
            const IndexData& GetIndexData() const noexcept;

            void SetVertexData(const std::vector<graphics::Vertex>& vertices);
            void SetIndexData(const std::vector<uint32_t>& indices);

        private:
            Base* m_base;
            GpuAllocator* m_allocator;
            VertexData m_vertexData;
            IndexData m_indexData;
    };
}