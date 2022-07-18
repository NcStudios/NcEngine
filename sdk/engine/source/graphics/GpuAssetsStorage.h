#pragma once

#include "assets/AssetManagers.h"
#include "graphics/GpuAssetsData.h"

namespace nc
{
namespace graphics
{
class Base;
class GpuAllocator;

class GpuAssetsStorage
{
    public:
        GpuAssetsStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        ~GpuAssetsStorage() noexcept;

        /** Meshes **/
        const VertexBuffer& GetVertexData() const noexcept;
        const IndexBuffer& GetIndexData() const noexcept;
        void UpdateMeshBuffer(const MeshBufferData& meshBufferData);

        /** Textures **/
        void UpdateTextureBuffer(const TextureBufferData& textureBufferData);

    private:
        Base* m_base;
        GpuAllocator* m_allocator;

        /** Meshes **/
        VertexBuffer m_vertexBuffer;
        IndexBuffer m_indexBuffer;
        nc::Connection<const MeshBufferData&> m_onMeshUpdateConnection;

        /** Textures **/
        void LoadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadAllTextureBuffer();

        std::vector<TextureBuffer> m_textureBuffers;
        std::vector<TextureImageInfo> m_textureImageInfos;
        vk::UniqueSampler m_sampler;
        nc::Connection<const TextureBufferData&> m_onTextureUpdateConnection;
};
}
}