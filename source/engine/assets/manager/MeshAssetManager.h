#pragma once

#include "assets/AssetService.h"
#include "utility/Signal.h"

#include "ncasset/AssetsFwd.h"

#include <string>
#include <unordered_map>

namespace nc
{
struct MeshUpdateEventData;

class MeshAssetManager : public IAssetService<MeshView, std::string>
{
    public:
        explicit MeshAssetManager(const std::string& assetDirectory);
        ~MeshAssetManager() noexcept;

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> MeshView override;
        bool IsLoaded(const std::string& path) const override;
        auto OnUpdate() -> Signal<const MeshUpdateEventData&>&;

    private:
        std::vector<asset::MeshVertex> m_vertexData;
        std::vector<uint32_t> m_indexData;
        std::unordered_map<std::string, MeshView> m_accessors;
        std::string m_assetDirectory;
        Signal<const MeshUpdateEventData&> m_onUpdate;

        void AddMeshView(const std::string& path, bool isExternal);
};
} // namespace nc
