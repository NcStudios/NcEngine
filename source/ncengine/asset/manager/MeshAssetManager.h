#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

namespace nc::asset
{
struct MeshUpdateEventData;
struct BoneUpdateEventData;

class MeshAssetManager : public IAssetService<MeshView, std::string>
{
    public:
        explicit MeshAssetManager(const std::string& assetDirectory);

        auto Load(const std::string& path)                     -> bool                          override;
        auto Load(std::span<const std::string> paths)          -> bool                          override;
        auto Unload(const std::string& path)                   -> bool                          override;
        void UnloadAll()                                                                        override; 
        auto Acquire(const std::string& path)            const -> MeshView                      override;
        auto Acquire(AssetId id)                         const -> MeshView                      override;
        auto GetAllLoaded(bool serializableOnly = false) const -> std::vector<std::string_view> override;
        auto IsLoaded(const std::string& path)           const -> bool                          override { return m_accessors.contains(path); }
        auto GetPath(AssetId id)                         const -> std::string_view              override { return m_accessors.key_at(id); }
        auto GetAssetType()                              const -> asset::AssetType              override { return asset::AssetType::Mesh; }
        auto OnBoneUpdate()                                    -> decltype(auto)                         { return (m_onBoneUpdate); }
        auto OnMeshUpdate()                                    -> decltype(auto)                         { return (m_onMeshUpdate); }

    private:
        std::vector<asset::MeshVertex> m_vertexData;
        std::vector<uint32_t> m_indexData;
        StringMap<MeshView> m_accessors;
        std::string m_assetDirectory;
        Signal<const asset::BoneUpdateEventData&> m_onBoneUpdate;
        Signal<const asset::MeshUpdateEventData&> m_onMeshUpdate;

        asset::Mesh ImportMesh(const std::string& path);
};
} // namespace nc::asset
