#pragma once

#include "assets/AssetService.h"

#include "ncasset/AssetsFwd.h"

#include <unordered_map>

namespace nc
{
class ConcaveColliderAssetManager : public IAssetService<ConcaveColliderView, std::string>
{
    public:
        explicit ConcaveColliderAssetManager(const std::string& concaveColliderAssetDirectory);

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> ConcaveColliderView override;
        bool IsLoaded(const std::string& path) const override;

    private:
        std::unordered_map<std::string, asset::ConcaveCollider> m_concaveColliders;
        std::string m_assetDirectory;
};
} // namespace nc
