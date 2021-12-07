#include "CubeMap.h"

namespace nc::graphics
{
    CubeMap::CubeMap(nc::graphics::Graphics* graphics, const std::array<stbi_uc*, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize, const std::string& uid)
    : m_base{ graphics->GetBasePtr() },
      m_memoryIndex { 0 },
      m_cubeMapImage { nullptr },
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
          m_memoryIndex{std::exchange(other.m_memoryIndex, 0)},
          m_cubeMapImage{std::exchange(other.m_cubeMapImage, nullptr)},
          m_cubeMapview{std::move(other.m_cubeMapview)},
          m_uid{std::move(other.m_uid)}
    {
    }

    CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
    {
        m_base = std::exchange(other.m_base, nullptr);
        m_memoryIndex = std::exchange(other.m_memoryIndex, 0);
        m_cubeMapImage = std::exchange(other.m_cubeMapImage, nullptr);
        m_cubeMapview = std::move(other.m_cubeMapview);
        m_uid = std::move(other.m_uid);

        return *this;
    }

    void CubeMap::Bind(const std::array<stbi_uc*, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize)
    {
        m_memoryIndex = m_base->CreateCubeMapTexture(pixels, width, height, cubeMapSize, &m_cubeMapImage);
        m_cubeMapview = m_base->CreateCubeMapTextureView(m_cubeMapImage);
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
        if (m_cubeMapImage)
        {
            m_base->DestroyImage(m_memoryIndex);
            m_cubeMapImage = nullptr;
        }

        m_base = nullptr;
        m_cubeMapview.reset();
    }
}