#pragma once

#include "service/ServiceLocator.h"

#include "ncasset/AssetType.h"

#include <span>
#include <string>

namespace nc::asset
{
struct Texture;

class IRuntimeTextureLoader
{
    public:
        IRuntimeTextureLoader()
        {
            ServiceLocator<IRuntimeTextureLoader>::Register(this);
        }

        virtual ~IRuntimeTextureLoader() = default;

        virtual auto LoadFromMemory(const std::string& key, Texture texture) -> bool = 0;
        virtual auto LoadFromRGBA(const std::string& key,
                                  std::span<const uint8_t> rgbaData,
                                  uint32_t width,
                                  uint32_t height,
                                  TextureFormat format = TextureFormat::RGBA8_UNORM) -> bool = 0;
};

using RuntimeTextureLoaderService = ServiceLocator<IRuntimeTextureLoader>;
} // namespace nc::asset
