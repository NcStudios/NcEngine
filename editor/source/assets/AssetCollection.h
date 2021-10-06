#pragma once

#include "Asset.h"

#include <span>
#include <vector>

namespace nc::editor
{
    class AssetCollection
    {
        public:
            void Add(Asset asset);
            bool Remove(const std::filesystem::path& assetPath);
            bool Contains(const std::filesystem::path& assetPath) const;
            auto View() const -> std::span<const Asset>;

        private:
            std::vector<Asset> m_assets;
    };
}