#pragma once

#include "AssetService.h"
#include "graphics/Vertex.h"
#include "utility/Signal.h"

#include <string>
#include <unordered_map>

namespace nc
{
    struct VertexData
    {
        std::vector<graphics::Vertex> vertices;
    };

    struct IndexData
    {
        std::vector<uint32_t> indices;
    };

    class MeshAssetManager : public IAssetService<MeshView, std::string>
    {
        public:
            MeshAssetManager(const std::string& assetDirectory);

            bool Load(const std::string& path, bool isExternal) override;
            bool Load(std::span<const std::string> paths, bool isExternal) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> MeshView override;
            bool IsLoaded(const std::string& path) const override;
            auto OnMeshAdd() -> Signal<const MeshAsset&>*;

        private:
            VertexData m_vertexData;
            IndexData m_indexData; 
            std::unordered_map<std::string, MeshView> m_accessors;
            std::string m_assetDirectory;
            Signal<const MeshAsset&> m_onMeshAdd;

            void UpdateBuffers();
    };
}