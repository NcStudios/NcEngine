#pragma once

#include "assets/AssetManagers.h"
#include "graphics/cubemaps/CubeMapStorage.h"
#include "graphics/textures/TextureStorage.h"
#include "graphics/meshes/MeshStorage.h"

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