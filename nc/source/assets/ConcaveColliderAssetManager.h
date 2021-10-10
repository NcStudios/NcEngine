#pragma once

#include "AssetService.h"

#include <unordered_map>

namespace nc
{
    struct ConcaveColliderFlyweight
    {
        std::vector<Triangle> triangles;
        float maxExtent;
    };

    class ConcaveColliderAssetManager : public IAssetService<ConcaveColliderView>
    {
        protected:
            bool Load(const std::string& path) override;
            bool Load(const std::vector<std::string>& paths) override;
            bool Unload(const std::string& path) override;
            auto Acquire(const std::string& path) const -> ConcaveColliderView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, ConcaveColliderFlyweight> m_concaveColliders;
    };
}