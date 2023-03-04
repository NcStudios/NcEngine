#pragma once

#include "graphics/api/vulkan/buffers/ImmutableBuffer.h"
#include "utility/Signal.h"

namespace nc
{
struct GpuAccessorSignals;
struct MeshBufferData;

namespace graphics
{
class GpuAllocator;

class MeshStorage
{
    public:
        MeshStorage(GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);

        void UpdateBuffer(const MeshBufferData& meshBufferData);

        auto GetVertexData() const noexcept -> const ImmutableBuffer&
        {
            return m_vertexBuffer;
        }

        auto GetIndexData() const noexcept -> const ImmutableBuffer&
        {
            return m_indexBuffer;
        }

    private:
        GpuAllocator* m_allocator;
        ImmutableBuffer m_vertexBuffer;
        ImmutableBuffer m_indexBuffer;
        nc::Connection<const MeshBufferData&> m_onMeshUpdateConnection;
};
} // namespace nc::graphics
}
