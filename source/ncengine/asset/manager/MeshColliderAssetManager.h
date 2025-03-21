#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>

namespace nc::asset
{
struct MeshColliderUpdateEventData;

class MeshColliderAssetManager : public IAssetService<MeshColliderView, std::string>
{
    public:
        explicit MeshColliderAssetManager(const std::string& meshColliderAssetDirectory);

        auto Load(const std::string& path,
                  AssetSubtype = AssetSubtype::None)  -> bool                          override;
        auto Load(std::span<const std::string> paths,
                  AssetSubtype = AssetSubtype::None)  -> bool                          override;
        auto Unload(const std::string& path)          -> bool                          override;
        void UnloadAll()                                                               override;
        auto Acquire(const std::string& path)   const -> MeshColliderView              override;
        auto Acquire(AssetId id)                const -> MeshColliderView              override;
        auto GetAllLoaded()                     const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)  const -> bool                          override { return m_map.contains(path); }
        auto GetPath(AssetId id)                const -> std::string_view              override { return m_map.at(m_map.index(id)); }
        auto GetAssetType()                     const -> AssetType                     override { return AssetType::MeshCollider; }
        auto OnUpdate()                               -> decltype(auto)                         { return (m_onUpdate); }

    private:
        StringTable m_map;
        std::string m_assetDirectory;
        Signal<const MeshColliderUpdateEventData&> m_onUpdate;
};
} // namespace nc::asset
