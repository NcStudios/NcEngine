#pragma once

#include "AssetService.h"
#include "assets/AssetData.h"
#include "graphics/meshes/Vertex.h"
#include "utility/Signal.h"

#include <string>
#include <unordered_map>

namespace nc
{
class MeshAssetManager : public IAssetService<MeshView, std::string>
{
    public:
        MeshAssetManager(const std::string& assetDirectory);
        ~MeshAssetManager() noexcept;

        bool Load(const std::string& path, bool isExternal) override;
        bool Load(std::span<const std::string> paths, bool isExternal) override;
        bool Unload(const std::string& path) override;
        void UnloadAll() override;
        auto Acquire(const std::string& path) const -> MeshView override;
        bool IsLoaded(const std::string& path) const override;
        auto OnUpdate() -> Signal<const MeshBufferData&>*;

    private:
        std::vector<Vertex> m_vertexData;
        std::vector<uint32_t> m_indexData; 
        std::unordered_map<std::string, MeshView> m_accessors;
        std::string m_assetDirectory;
        Signal<const MeshBufferData&> m_onUpdate;
};
} // namespace nc