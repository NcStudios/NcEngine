#pragma once

#include "meshes/MeshStorage.h"

namespace nc::graphics
{
namespace vulkan
{
struct GpuAssetsStorage
{
    GpuAssetsStorage(vk::Device device,
                     nc::graphics::GpuAllocator* allocator,
                     Signal<const asset::MeshUpdateEventData&>& onMeshUpdate);

    MeshStorage meshStorage;
};
} // namespace vulkan
} // namespace nc::graphics
