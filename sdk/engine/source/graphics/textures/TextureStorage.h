#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/Textures.h"

namespace nc::graphics
{
class Base;
class GpuAllocator;

class TextureStorage
{
    public:
        TextureStorage(Base* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        ~TextureStorage() noexcept;
        void UpdateBuffer(const TextureBufferData& textureBufferData);

    private:
        Base* m_base;
        GpuAllocator* m_allocator;
        std::vector<TextureBuffer> m_textureBuffers;
        std::vector<TextureImageInfo> m_textureImageInfos;
        vk::UniqueSampler m_sampler;
        nc::Connection<const TextureBufferData&> m_onTextureUpdateConnection;
};
}