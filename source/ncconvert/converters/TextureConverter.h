#pragma once

#include "ncasset/AssetsFwd.h"

#include <filesystem>

namespace nc::convert
{
class TextureConverter
{
    public:
        auto ImportCubeMap(const std::filesystem::path& path) -> asset::CubeMap;
        auto ImportTexture(const std::filesystem::path& path) -> asset::Texture;
};

class Image
{
    static constexpr int32_t numChannels = 4;

    public:
        explicit Image(const std::filesystem::path& path);

        explicit Image(unsigned char* data, int32_t width, int32_t height)
            : m_data{data}, m_width{width}, m_height{height}
        {
        }

        ~Image() noexcept;

        auto GetSizeInBytes()         const -> uint32_t;
        auto MakeTextureSubResource() const -> asset::TextureSubResource;

        // should be const
        auto Compress(asset::TextureFormat format,
                      asset::CompressionQuality quality) const -> asset::TextureSubResource;

    private:
        unsigned char* m_data = nullptr;
        int32_t m_width = 0;
        int32_t m_height = 0;
};
} // namespace nc::convert
