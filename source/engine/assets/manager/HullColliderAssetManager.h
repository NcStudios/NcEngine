#pragma once

#include "assets/AssetService.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc
{
class HullColliderAssetManager : public IAssetService<ConvexHullView, std::string>
{
    public:
        explicit HullColliderAssetManager(const std::string& assetDirectory);

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> ConvexHullView override;
        bool IsLoaded(const std::string& path) const override;

    private:
        std::unordered_map<std::string, asset::HullCollider> m_hullColliders;
        std::string m_assetDirectory;
};
} // namespace nc
