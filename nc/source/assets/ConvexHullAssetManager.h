#pragma once

#include "AssetService.h"

#include <unordered_map>
#include <vector>

namespace nc
{
    struct ConvexHullFlyweight
    {
        std::vector<Vector3> vertices;
        Vector3 extents;
        float maxExtent;
    };

    class ConvexHullAssetManager : public IAssetService<ConvexHullView, std::string>
    {
        public:
            ConvexHullAssetManager(const std::string& assetDirectory);
            bool Load(const std::string& path, bool isExternal) override;
            bool Load(std::span<const std::string> paths, bool isExternal) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> ConvexHullView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, ConvexHullFlyweight> m_hullColliders;
            std::string m_assetDirectory;
    };
}