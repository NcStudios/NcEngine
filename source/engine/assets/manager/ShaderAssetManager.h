#pragma once

#include "assets/AssetService.h"

#include <string>
#include <vector>

namespace nc
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

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> ShaderView override;
        bool IsLoaded(const std::string& path) const override;

    private:
        std::vector<ShaderFlyweight> m_shaderFlyweights;
        std::string m_assetDirectory;
};
} // namespace nc
