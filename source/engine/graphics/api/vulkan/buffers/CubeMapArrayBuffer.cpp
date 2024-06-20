#include "CubeMapArrayBuffer.h"
#include "asset/AssetData.h"
#include "graphics/api/vulkan/Initializers.h"

namespace
{

}
namespace nc::graphics::vulkan
{
CubeMap::CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data)
    : m_image{allocator->CreateCubeMapTexture(data.cubeMap.pixelData.data(), static_cast<uint32_t>(data.cubeMap.pixelData.size()), data.cubeMap.faceSideLength)},
      m_cubeMapView{allocator->CreateCubeMapTextureView(m_image)},
      m_faceViews{allocator->CreateCubeMapFaceViews(m_image)} {}

void CubeMap::Clear() noexcept
{
    m_image.Release();
    m_cubeMapView.reset();
    m_faceViews.clear();
}

auto CubeMap::GetFaceViews() const noexcept -> std::vector<vk::ImageView>
{
    auto faceViews = std::vector<vk::ImageView>{};
    faceViews.reserve(6);

    std::ranges::transform(m_faceViews, std::back_inserter(faceViews), [](auto& faceView){ return faceView.get(); });
    return faceViews;
}

CubeMapArrayBuffer::CubeMapArrayBuffer(vk::Device device)
    : sampler{CreateTextureSampler(device, vk::SamplerAddressMode::eRepeat)}
{}
} // namespace nc::graphics::vulkan
