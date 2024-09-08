#pragma once

#include "asset/AssetService.h"
#include "utility/StringMap.h"
#include "ncengine/utility/Signal.h"

#include "ncasset/AssetsFwd.h"

namespace nc::asset
{
struct MeshUpdateEventData;
struct BoneUpdateEventData;
struct NamedMesh;

class MeshAssetManager : public IAssetService<MeshView, std::string>
{
    public:
        explicit MeshAssetManager(const std::string& assetDirectory);

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> MeshView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetPath(size_t id) const -> std::string_view override { return m_accessors.key_at(id); }
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto GetAssetType() const noexcept -> asset::AssetType override { return asset::AssetType::Mesh; }
        auto OnBoneUpdate() -> Signal<const asset::BoneUpdateEventData&>&;
        auto OnMeshUpdate() -> Signal<const asset::MeshUpdateEventData&>&;
        auto GetMeshData(std::string_view path) const -> NamedMesh;

    private:
        std::vector<asset::MeshVertex> m_vertexData;
        std::vector<uint32_t> m_indexData;
        StringMap<MeshView> m_accessors;
        std::string m_assetDirectory;
        Signal<const asset::BoneUpdateEventData&> m_onBoneUpdate;
        Signal<const asset::MeshUpdateEventData&> m_onMeshUpdate;

        asset::Mesh ImportMesh(const std::string& path, bool isExternal);
};
} // namespace nc::asset
