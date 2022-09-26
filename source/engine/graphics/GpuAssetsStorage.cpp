#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : cubeMapStorage{base, allocator, gpuAccessorSignals},
      meshStorage{allocator, gpuAccessorSignals},
      textureStorage{base, allocator, gpuAccessorSignals}
{
}
} // namespace nc::graphics