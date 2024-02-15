#pragma once

#include "cubemaps/CubeMapStorage.h"
#include "meshes/MeshStorage.h"

namespace nc::graphics
{
namespace vulkan
{
struct GpuAssetsStorage
{
    GpuAssetsStorage(vk::Device device,
                     nc::graphics::GpuAllocator* allocator,
                     Signal<const asset::CubeMapUpdateEventData&>& onCubeMapUpdate,
                     Signal<const asset::MeshUpdateEventData&>& onMeshUpdate);

    CubeMapStorage cubeMapStorage;
    MeshStorage meshStorage;
};
} // namespace vulkan
} // namespace nc::graphics
