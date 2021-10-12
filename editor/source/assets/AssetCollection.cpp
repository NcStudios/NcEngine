#include "AssetCollection.h"
#include "utility/Output.h"

#include <algorithm>
#include <fstream>

namespace nc::editor
{
    void AssetCollection::Add(Asset asset)
    {
        m_assets.push_back(std::move(asset));
    }

    bool AssetCollection::Remove(const std::filesystem::path& assetPath)
    {
        auto pos = std::ranges::find_if(m_assets, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        if(pos == m_assets.end())
        {
            Output::Log("AssetCollection::Remove - Asset does not exist: " + assetPath.string());
            return false;
        }

        /** @todo unload once implemented */

        *pos = std::move(m_assets.back());
        m_assets.pop_back();
        return true;
    }
    
    bool AssetCollection::Contains(const std::filesystem::path& assetPath) const
    {
        auto pos = std::ranges::find_if(m_assets, [&assetPath](const auto& asset)
        {
            return asset.sourcePath == assetPath;
        });

        return pos != m_assets.end();
    }
    
    bool AssetCollection::ContainsNca(const std::filesystem::path& assetPath) const
    {
        auto pos = std::ranges::find_if(m_assets, [&assetPath](const auto& asset)
        {
            if(asset.ncaPath.has_value())
                return asset.ncaPath.value() == assetPath;
            
            return false;
        });

        return pos != m_assets.end();
    }

    auto AssetCollection::View() const -> std::span<const Asset>
    {
        return std::span<const Asset>{m_assets};
    }
}