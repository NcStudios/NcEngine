#include "GpuAssetsStorage.h"

namespace nc::graphics::vulkan
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device,
                                   nc::graphics::GpuAllocator* allocator,
                                   Signal<const asset::MeshUpdateEventData&>& onMeshUpdate)
    : meshStorage{allocator, onMeshUpdate}
{
}
} // namespace nc::graphics::vulkan
