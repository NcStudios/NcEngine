#pragma once

#include "graphics/api/vulkan/buffers/ImmutableBuffer.h"
#include "utility/Signal.h"

namespace nc
{
namespace asset
{
struct MeshUpdateEventData;
} // namespace asset

namespace graphics
{
class GpuAllocator;

class MeshStorage
{
    public:
        MeshStorage(GpuAllocator* allocator, Signal<const asset::MeshUpdateEventData&>& onMeshUpdate);

        void UpdateBuffer(const asset::MeshUpdateEventData& eventData);

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
        nc::Connection<const asset::MeshUpdateEventData&> m_onMeshUpdateConnection;
};
} // namespace nc::graphics
}
