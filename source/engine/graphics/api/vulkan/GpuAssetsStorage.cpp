#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device device,
                                   GpuAllocator* allocator,
                                   Signal<const CubeMapBufferData&>& onCubeMapUpdate,
                                   Signal<const MeshBufferData&>& onMeshUpdate,
                                   Signal<const TextureBufferData&>& onTextureUpdate)
    : cubeMapStorage{device, allocator, onCubeMapUpdate},
      meshStorage{allocator, onMeshUpdate},
      textureStorage{device, allocator, onTextureUpdate}
{
}
} // namespace nc::graphics
