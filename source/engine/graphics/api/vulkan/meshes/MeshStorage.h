#pragma once

#include "graphics/api/vulkan/buffers/MeshBuffer.h"
#include "utility/Signal.h"

namespace nc
{
namespace asset
{
struct MeshUpdateEventData;
} // namespace asset

namespace graphics::vulkan
{
class GpuAllocator;

class MeshStorage
{
    public:
        MeshStorage(GpuAllocator* allocator, Signal<const asset::MeshUpdateEventData&>& onMeshUpdate);

        void UpdateBuffer(const asset::MeshUpdateEventData& eventData);

        auto GetVertexData() const noexcept -> const MeshBuffer&
        {
            return m_vertexBuffer;
        }

        auto GetIndexData() const noexcept -> const MeshBuffer&
        {
            return m_indexBuffer;
        }

    private:
        GpuAllocator* m_allocator;
        MeshBuffer m_vertexBuffer;
        MeshBuffer m_indexBuffer;
        nc::Connection<const asset::MeshUpdateEventData&> m_onMeshUpdateConnection;
};
} // namespace nc::graphics::vulkan
}
