#pragma once

#include "ncasset/AssetType.h"

#include <memory>

namespace nc::convert
{
struct Target;
class AudioConverter;
class GeometryConverter;
class TextureConverter;

/** @brief Manager that handles nca conversion and serialization. */
class Builder
{
    public:
        Builder();
        ~Builder() noexcept;

        /** @brief Create a new .nca file. */
        auto Build(asset::AssetType type, const Target& target) -> bool;

    private:
        std::unique_ptr<AudioConverter> m_audioConverter;
        std::unique_ptr<GeometryConverter> m_geometryConverter;
        std::unique_ptr<TextureConverter> m_textureConverter;
};
} // namespace nc::convert
