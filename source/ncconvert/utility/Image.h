#pragma once

#include "ncasset/AssetsFwd.h"

#include <cstdlib>
#include <filesystem>
#include <memory>

namespace nc::convert
{
struct RawImageDeleter
{
    void operator()(unsigned char* ptr) const
    {
        std::free(ptr);
    }
};

using RawImage = std::unique_ptr<unsigned char, RawImageDeleter>;

auto MakeRawImage(int width,
                  int height,
                  const unsigned char fillRGBA[4]) -> RawImage;

class Image
{
    public:
        static constexpr int           numChannels              = 4;
        static constexpr int           minCompressionDimensions = 4;
        static constexpr unsigned char paddingRGBA[4]           = {0, 0, 0, 255};

        explicit Image(const std::filesystem::path& path);

        explicit Image(int width,
                       int height,
                       const unsigned char fillRGBA[4] = paddingRGBA)
            : m_data{MakeRawImage(width, height, fillRGBA)},
              m_width{width},
              m_height{height}
        {
        }

        explicit Image(RawImage&& data,
                       int width,
                       int height)
            : m_data{std::move(data)},
              m_width{width},
              m_height{height}
        {
        }

        auto GetData()                                   const -> unsigned char*             { return m_data.get(); }
        auto GetWidth()                                  const -> int                        { return m_width; }
        auto GetHeight()                                 const -> int                        { return m_height; }
        auto GetSizeInBytes()                            const -> uint32_t                   { return m_width * m_height * numChannels; }
        auto RequiresCompressionPadding()                const -> bool                       { return m_width % 4 != 0 || m_height % 4 != 0; }
        auto MakeTextureSubResource()                    const -> asset::TextureSubResource;
        auto Compress(asset::TextureFormat format)       const -> asset::TextureSubResource;

        // Resize using up/down sampling (mitchell filter/cubic interpolation).
        void Resize(int width, int height);

        // Resize for use with BCn compression. No-op if RequiresCompressionPadding() == false. Dimensions are forced
        // to closest powers of two (and at least 4x4). The original pixels are unchanged and start at top left (coords
        // unchanged). Padding pixels are black with full alpha.
        void ResizePadded();

    private:
        RawImage m_data = nullptr;
        int m_width = 0;
        int m_height = 0;
};
} // namespace nc::convert
