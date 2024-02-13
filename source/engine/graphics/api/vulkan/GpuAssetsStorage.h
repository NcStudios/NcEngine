#pragma once

#include "cubemaps/CubeMapStorage.h"
#include "textures/TextureStorage.h"
#include "meshes/MeshStorage.h"

namespace nc::graphics
{
class GpuAllocator;
namespace vulkan
{
struct GpuAssetsStorage
{
    GpuAssetsStorage(vk::Device device,
                     GpuAllocator* allocator,
                     Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate,
                     Signal<const asset::MeshUpdateEventData&>& onMeshUpdate,
                     Signal<const asset::TextureUpdateEventData&>& onTextureUpdate);

    CubeMapStorage cubeMapStorage;
    MeshStorage meshStorage;
    TextureStorage textureStorage;
};
} // namespace vulkan
} // namespace nc::graphics
