#pragma once

#include "AssetService.h"
#include "graphics/Resources/ImmutableBuffer.h"

#include <string>
#include <unordered_map>

namespace nc
{
    struct VertexData
    {
        graphics::ImmutableBuffer<graphics::Vertex> buffer;
        std::vector<graphics::Vertex> vertices;
    };

    struct IndexData
    {
        graphics::ImmutableBuffer<uint32_t> buffer;
        std::vector<uint32_t> indices;
    };

    class MeshAssetManager : public IAssetService<MeshView>
    {
        public:
            MeshAssetManager(graphics::Graphics* graphics);
            ~MeshAssetManager() noexcept;

            bool Load(const std::string& path) override;
            bool Load(const std::vector<std::string>& paths) override;
            bool Unload(const std::string& path) override;
            auto Acquire(const std::string& path) const -> MeshView override;
            bool IsLoaded(const std::string& path) const override;

            auto GetVertexBuffer() noexcept -> vk::Buffer* { return m_vertexData.buffer.GetBuffer(); }
            auto GetIndexBuffer() noexcept -> vk::Buffer* { return m_indexData.buffer.GetBuffer(); }

        private:
            graphics::Graphics* m_graphics;
            VertexData m_vertexData;
            IndexData m_indexData; 
            std::unordered_map<std::string, MeshView> m_accessors;

            void UpdateBuffers();
    };
}