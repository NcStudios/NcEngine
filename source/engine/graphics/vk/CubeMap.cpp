#include "CubeMap.h"
#include "graphics/GpuOptions.h"

namespace nc::graphics
{
    CubeMap::CubeMap(GpuOptions* base, GpuAllocator* allocator, const std::array<unique_c_ptr<unsigned char[]>, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize, const std::string& uid)
        : m_base{base},
          m_allocator{allocator},
          m_image{},
          m_cubeMapview{},
          m_uid{uid}
    {
        Bind(pixels, width, height, cubeMapSize);
    }

    CubeMap::~CubeMap() noexcept
    {
        Clear();
    }

    CubeMap::CubeMap(CubeMap&& other) noexcept
        : m_base{std::exchange(other.m_base, nullptr)},
          m_allocator{std::exchange(other.m_allocator, nullptr)},
          m_image{std::exchange(other.m_image, GpuAllocation<vk::Image>{})},
          m_cubeMapview{std::move(other.m_cubeMapview)},
          m_uid{std::move(other.m_uid)}
    {
    }

    CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_allocator = std::exchange(other.m_allocator, nullptr);
        m_image = std::exchange(other.m_image, GpuAllocation<vk::Image>{});
        m_cubeMapview = std::move(other.m_cubeMapview);
        m_uid = std::move(other.m_uid);

        return *this;
    }

    void CubeMap::Bind(const std::array<unique_c_ptr<unsigned char[]>, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize)
    {
        m_image = m_allocator->CreateCubeMapTexture(pixels, width, height, cubeMapSize);
        m_cubeMapview = CreateCubeMapTextureView(m_base->GetDevice(), m_image);
    }

    const vk::ImageView& CubeMap::GetImageView() const noexcept
    {
        return m_cubeMapview.get();
    }

    const std::string& CubeMap::GetUid() const noexcept
    {
        return m_uid;
    }

    void CubeMap::Clear() noexcept
    {
        m_base = nullptr;
        m_image.Release();
        m_cubeMapview.reset();
    }
}