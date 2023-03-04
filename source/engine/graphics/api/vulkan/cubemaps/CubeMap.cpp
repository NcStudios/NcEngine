#include "CubeMap.h"

#include "assets/AssetData.h"

namespace nc::graphics
{
    CubeMap::CubeMap(vk::Device device, GpuAllocator* allocator, const CubeMapData& data)
        : m_device{device},
          m_allocator{allocator},
          m_image{},
          m_cubeMapView{},
          m_uid{data.id}
    {
        Bind(data);
    }

    CubeMap::~CubeMap() noexcept
    {
        Clear();
    }

    CubeMap::CubeMap(CubeMap&& other) noexcept
        : m_device{std::exchange(other.m_device, nullptr)},
          m_allocator{std::exchange(other.m_allocator, nullptr)},
          m_image{std::exchange(other.m_image, GpuAllocation<vk::Image>{})},
          m_cubeMapView{std::move(other.m_cubeMapView)},
          m_uid{std::move(other.m_uid)}
    {
    }

    CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
    {
        m_device = std::exchange(other.m_device, nullptr);
        m_allocator = std::exchange(other.m_allocator, nullptr);
        m_image = std::exchange(other.m_image, GpuAllocation<vk::Image>{});
        m_cubeMapView = std::move(other.m_cubeMapView);
        m_uid = std::move(other.m_uid);

        return *this;
    }

    void CubeMap::Bind(const CubeMapData& data)
    {
        const auto& cubeMap = data.cubeMap;
        m_image = m_allocator->CreateCubeMapTexture(cubeMap.pixelData.data(), static_cast<uint32_t>(cubeMap.pixelData.size()), cubeMap.faceSideLength);
        m_cubeMapView = CreateCubeMapTextureView(m_device, m_image);
    }

    const vk::ImageView& CubeMap::GetImageView() const noexcept
    {
        return m_cubeMapView.get();
    }

    const std::string& CubeMap::GetUid() const noexcept
    {
        return m_uid;
    }

    void CubeMap::Clear() noexcept
    {
        m_image.Release();
        m_cubeMapView.reset();
    }
}