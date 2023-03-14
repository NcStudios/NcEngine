#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device device,
                                   GpuAllocator* allocator,
                                   Signal<const CubeMapUpdateEventData&>& onCubeMapUpdate,
                                   Signal<const MeshUpdateEventData&>& onMeshUpdate,
                                   Signal<const TextureUpdateEventData&>& onTextureUpdate)
    : cubeMapStorage{device, allocator, onCubeMapUpdate},
      meshStorage{allocator, onMeshUpdate},
      textureStorage{device, allocator, onTextureUpdate}
{
}
} // namespace nc::graphics
