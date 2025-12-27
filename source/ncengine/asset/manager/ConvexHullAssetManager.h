#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>

namespace nc::asset
{
struct ConvexHullUpdateEventData;

class ConvexHullAssetManager : public IAssetService<ConvexHullView, std::string>
{
    public:
        explicit ConvexHullAssetManager(const std::string& assetDirectory);

        auto Load(const std::string& path)                     -> bool                          override;
        auto Load(std::span<const std::string> paths)          -> bool                          override;
        auto Unload(const std::string& path)                   -> bool                          override;
        void UnloadAll()                                                                        override;
        auto Acquire(const std::string& path)            const -> ConvexHullView                override;
        auto Acquire(AssetId id)                         const -> ConvexHullView                override;
        auto GetAllLoaded(bool serializableOnly = false) const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)           const -> bool                          override { return m_map.contains(path); }
        auto GetPath(AssetId id)                         const -> std::string_view              override { return m_map.at(m_map.index(id)); }
        auto GetAssetType()                              const -> asset::AssetType              override { return asset::AssetType::ConvexHull; }
        auto OnUpdate()                                        -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_map;
        std::string m_assetDirectory;
        Signal<const ConvexHullUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
