#include "GpuAssetsStorage.h"

namespace nc::graphics::vulkan
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device device,
                                   GpuAllocator* allocator,
                                   Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate,
                                   Signal<const asset::MeshUpdateEventData&>& onMeshUpdate,
                                   Signal<const asset::TextureUpdateEventData&>& onTextureUpdate)
    : cubeMapStorage{device, allocator, onCubeMapUpdate},
      meshStorage{allocator, onMeshUpdate},
      textureStorage{device, allocator, onTextureUpdate}
{
}
} // namespace nc::graphics::vulkan
