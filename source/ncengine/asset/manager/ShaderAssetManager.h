#pragma once

#include "asset/AssetService.h"

#include <string>
#include <vector>

namespace nc::asset
{
struct ShaderFlyweight
{
    std::string uid;
    std::vector<uint32_t> vertexByteCode;
    std::vector<uint32_t> fragmentByteCode;
    std::vector<DescriptorManifest> descriptors;
};

class ShaderAssetManager final : public IAssetService<ShaderView, std::string>
{
    public:
        explicit ShaderAssetManager(const std::string& assetDirectory);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> ShaderView override;
        auto GetPath(size_t) const -> std::string_view override { throw NcError{"Not Implemented"};}
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::Shader; }
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;

    private:
        std::vector<ShaderFlyweight> m_shaderFlyweights;
        std::string m_assetDirectory;
};
} // namespace nc::asset
