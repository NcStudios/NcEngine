#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(vk::Device device, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : cubeMapStorage{device, allocator, gpuAccessorSignals},
      meshStorage{allocator, gpuAccessorSignals},
      textureStorage{device, allocator, gpuAccessorSignals}
{
}
} // namespace nc::graphics