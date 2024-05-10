#include "CubeMapArrayBuffer.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
CubeMap::CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data)
    : m_image{allocator->CreateCubeMapTexture(data.cubeMap.pixelData.data(), static_cast<uint32_t>(data.cubeMap.pixelData.size()), data.cubeMap.faceSideLength)},
      m_cubeMapView{allocator->CreateCubeMapTextureView(m_image)}{}

void CubeMap::Clear() noexcept
{
    m_image.Release();
    m_cubeMapView.reset();
}

CubeMapArrayBuffer::CubeMapArrayBuffer(vk::Device device)
    : sampler{CreateTextureSampler(device, vk::SamplerAddressMode::eRepeat)}
{}
} // namespace nc::graphics::vulkan
