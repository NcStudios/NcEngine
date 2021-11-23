#pragma once

#include "AssetService.h"

#include <unordered_map>
#include <vector>

namespace nc
{
    struct ConcaveColliderFlyweight
    {
        std::vector<Triangle> triangles;
        float maxExtent;
    };

    class ConcaveColliderAssetManager : public IAssetService<ConcaveColliderView, std::string>
    {
        public:
            bool Load(const std::string& path) override;
            bool Load(std::span<const std::string> paths) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> ConcaveColliderView override;
            bool IsLoaded(const std::string& path) const override;
        
        private:
            std::unordered_map<std::string, ConcaveColliderFlyweight> m_concaveColliders;
    };
}