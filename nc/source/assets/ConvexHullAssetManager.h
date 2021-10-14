#pragma once

#include "AssetService.h"

#include <unordered_map>

namespace nc
{
    struct ConvexHullFlyweight
    {
        std::vector<Vector3> vertices;
        Vector3 extents;
        float maxExtent;
    };

    class ConvexHullAssetManager : public IAssetService<ConvexHullView>
    {
        protected:
            bool Load(const std::string& path) override;
            bool Load(const std::vector<std::string>& paths) override;
            bool Unload(const std::string& path) override;
            auto Acquire(const std::string& path) const -> ConvexHullView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, ConvexHullFlyweight> m_hullColliders;
    };
}