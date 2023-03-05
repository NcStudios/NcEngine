#pragma once

#include "assets/AssetManagers.h"
#include "cubemaps/CubeMapStorage.h"
#include "textures/TextureStorage.h"
#include "meshes/MeshStorage.h"

namespace nc::graphics
{
class GpuAllocator;

struct GpuAssetsStorage
{
    GpuAssetsStorage(vk::Device device, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
    CubeMapStorage cubeMapStorage;
    MeshStorage meshStorage;
    TextureStorage textureStorage;
};
} // namespace nc::graphics