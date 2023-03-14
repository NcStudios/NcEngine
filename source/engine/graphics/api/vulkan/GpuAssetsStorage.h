#pragma once

#include "cubemaps/CubeMapStorage.h"
#include "textures/TextureStorage.h"
#include "meshes/MeshStorage.h"

namespace nc::graphics
{
class GpuAllocator;

struct GpuAssetsStorage
{
    GpuAssetsStorage(vk::Device device,
                     GpuAllocator* allocator,
                     Signal<const CubeMapBufferData&>& onCubeMapUpdate,
                     Signal<const MeshBufferData&>& onMeshUpdate,
                     Signal<const TextureBufferData&>& onTextureUpdate);

    CubeMapStorage cubeMapStorage;
    MeshStorage meshStorage;
    TextureStorage textureStorage;
};
} // namespace nc::graphics