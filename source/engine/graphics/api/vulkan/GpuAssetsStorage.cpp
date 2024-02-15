#include "GpuAssetsStorage.h"

namespace nc::graphics::vulkan
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device device,
                                   nc::graphics::GpuAllocator* allocator,
                                   Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate,
                                   Signal<const asset::MeshUpdateEventData&>& onMeshUpdate)
    : cubeMapStorage{device, allocator, onCubeMapUpdate},
      meshStorage{allocator, onMeshUpdate}
{
}
} // namespace nc::graphics::vulkan
