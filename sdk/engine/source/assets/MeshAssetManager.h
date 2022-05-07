#pragma once

#include "AssetService.h"
#include "graphics/Resources/ImmutableBuffer.h"

#include <string>
#include <unordered_map>

namespace nc
{
    namespace graphics { class Graphics; }

    struct VertexData
    {
        graphics::ImmutableBuffer buffer;
        std::vector<graphics::Vertex> vertices;
    };

    struct IndexData
    {
        graphics::ImmutableBuffer buffer;
        std::vector<uint32_t> indices;
    };

    class MeshAssetManager : public IAssetService<MeshView, std::string>
    {
        public:
            MeshAssetManager(graphics::Graphics* graphics, const std::string& assetDirectory);
            ~MeshAssetManager() noexcept;

            bool Load(const std::string& path, bool isExternal) override;
            bool Load(std::span<const std::string> paths, bool isExternal) override;
            bool Unload(const std::string& path) override;
            void UnloadAll() override;
            auto Acquire(const std::string& path) const -> MeshView override;
            bool IsLoaded(const std::string& path) const override;

            auto GetVertexBuffer() const noexcept -> vk::Buffer { return m_vertexData.buffer.GetBuffer(); }
            auto GetIndexBuffer() const noexcept -> vk::Buffer { return m_indexData.buffer.GetBuffer(); }

        private:
            graphics::Graphics* m_graphics;
            VertexData m_vertexData;
            IndexData m_indexData; 
            std::unordered_map<std::string, MeshView> m_accessors;
            std::string m_assetDirectory;

            void UpdateBuffers();
    };
}