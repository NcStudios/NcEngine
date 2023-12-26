#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <unordered_map>

namespace nc
{
namespace asset
{
struct MeshUpdateEventData;
struct BoneUpdateEventData;
} // namespace asset

class MeshAssetManager : public IAssetService<MeshView, std::string>
{
    public:
        explicit MeshAssetManager(const std::string& assetDirectory);
        ~MeshAssetManager() noexcept;

        bool Load(const std::string& path, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Load(std::span<const std::string> paths, bool isExternal, asset_flags_type flags = AssetFlags::None) override;
        bool Unload(const std::string& path, asset_flags_type flags = AssetFlags::None) override;
        void UnloadAll(asset_flags_type flags = AssetFlags::None) override;
        auto Acquire(const std::string& path, asset_flags_type flags = AssetFlags::None) const -> MeshView override;
        bool IsLoaded(const std::string& path, asset_flags_type flags = AssetFlags::None) const override;
        auto GetAllLoaded() const -> std::vector<std::string_view> override;
        auto OnBoneUpdate() -> Signal<const asset::BoneUpdateEventData&>&;
        auto OnMeshUpdate() -> Signal<const asset::MeshUpdateEventData&>&;

    private:
        std::vector<asset::MeshVertex> m_vertexData;
        std::vector<asset::BonesData> m_bonesData;
        std::vector<uint32_t> m_indexData;
        std::unordered_map<std::string, MeshView> m_accessors;
        std::string m_assetDirectory;
        Signal<const asset::BoneUpdateEventData&> m_onBoneUpdate;
        Signal<const asset::MeshUpdateEventData&> m_onMeshUpdate;

        asset::Mesh ImportMesh(const std::string& path, bool isExternal);
};
} // namespace nc
