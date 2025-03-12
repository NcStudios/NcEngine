#pragma once

#include <filesystem>
#include <optional>

namespace nc::convert
{
/** @brief Indicates special properties of asset. */
enum class AssetSubtype : uint8_t
{
    None,
    DiffuseTexture,
    NormalTexture,
    ParticleTexture,
    EffectTexture
};

/** @brief Target-specific options */
struct TargetOptions
{
    bool optimizeMesh = false;
    AssetSubtype subtype = AssetSubtype::None;
};

/** @brief Data describing a required asset conversion. */
struct Target
{
    Target(std::filesystem::path source,
           std::filesystem::path destination,
           std::optional<std::string> subResource = std::nullopt,
           TargetOptions options_ = TargetOptions{})
        : sourcePath{std::move(source)},
          destinationPath{std::move(destination)},
          subResourceName{std::move(subResource)},
          options{options_}
    {
    }

    std::filesystem::path sourcePath;
    std::filesystem::path destinationPath;
    std::optional<std::string> subResourceName;
    TargetOptions options;
};

/** @brief Data describing an asset for source code generation. */
struct ReflectedTarget
{
    std::string name;
    std::filesystem::path path;
    AssetSubtype subtype = AssetSubtype::None;
};
}
