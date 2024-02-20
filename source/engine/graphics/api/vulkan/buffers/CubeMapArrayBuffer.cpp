#include "CubeMapArrayBuffer.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
CubeMap::CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data)
    : m_image{allocator->CreateCubeMapTexture(data.cubeMap.pixelData.data(), static_cast<uint32_t>(data.cubeMap.pixelData.size()), data.cubeMap.faceSideLength)},
      m_cubeMapView{allocator->CreateCubeMapTextureView(m_image)}{}

CubeMap::CubeMap(CubeMap&& other) noexcept
        : m_image{std::exchange(other.m_image, GpuAllocation<vk::Image>{})},
          m_cubeMapView{std::move(other.m_cubeMapView)}
    {
    }

CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
{
    m_image = std::exchange(other.m_image, GpuAllocation<vk::Image>{});
    m_cubeMapView = std::move(other.m_cubeMapView);
    return *this;
}

CubeMap::~CubeMap() noexcept
{
    Clear();
}

const vk::ImageView& CubeMap::GetImageView() const noexcept
{
    return m_cubeMapView.get();
}

void CubeMap::Clear() noexcept
{
    m_image.Release();
    m_cubeMapView.reset();
}

CubeMapArrayBuffer::CubeMapArrayBuffer(vk::Device device)
    : sampler{CreateTextureSampler(device, vk::SamplerAddressMode::eRepeat)}
{
}
} // namespace nc::graphics::vulkan
