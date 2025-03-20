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

        bool Load(const std::string& path, AssetSubtype = AssetSubtype::None) override;
        bool Load(std::span<const std::string> paths, AssetSubtype = AssetSubtype::None) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> MeshView override;
        auto Acquire(AssetId id) const -> MeshView override;
        bool IsLoaded(const std::string& path) const override;
        auto GetPath(AssetId id) const -> std::string_view override { return m_accessors.key_at(id); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::Mesh; }
        auto OnBoneUpdate() -> Signal<const asset::BoneUpdateEventData&>&;
        auto OnMeshUpdate() -> Signal<const asset::MeshUpdateEventData&>&;

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
