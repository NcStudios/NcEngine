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

        auto Load(const std::string& path,
                  AssetSubtype = AssetSubtype::None)       -> bool                          override;
        auto Load(std::span<const std::string> paths,
                  AssetSubtype = AssetSubtype::None)       -> bool                          override;
        auto Unload(const std::string& path)               -> bool                          override;
        void UnloadAll()                                                                    override;
        auto Acquire(const std::string& path)        const -> ShaderView                    override;
        auto Acquire(AssetId)                        const -> ShaderView                    override { throw NcError{"Not Implemented"}; }
        auto IsLoaded(const std::string& path)       const -> bool                          override;
        auto GetAllLoaded()                          const -> std::vector<std::string_view> override;
        auto GetPath(AssetId)                        const -> std::string_view              override { throw NcError{"Not Implemented"}; }
        auto GetAssetType()                          const -> asset::AssetType              override { return asset::AssetType::Shader; }

    private:
        std::vector<ShaderFlyweight> m_shaderFlyweights;
        std::string m_assetDirectory;
};
} // namespace nc::asset
