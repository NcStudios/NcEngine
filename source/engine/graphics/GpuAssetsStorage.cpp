#include "GpuAssetsStorage.h"

namespace nc::graphics
{
GpuAssetsStorage::GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals)
    : meshStorage{base, allocator, gpuAccessorSignals},
      textureStorage{base, allocator, gpuAccessorSignals}
{
}
} // namespace nc::graphics