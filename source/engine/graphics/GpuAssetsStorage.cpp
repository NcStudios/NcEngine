#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(GpuOptions* gpuOptions, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : cubeMapStorage{gpuOptions, allocator, gpuAccessorSignals},
      meshStorage{allocator, gpuAccessorSignals},
      textureStorage{gpuOptions, allocator, gpuAccessorSignals}
{
}
} // namespace nc::graphics