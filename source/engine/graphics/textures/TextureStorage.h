#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/vk/Textures.h"

namespace nc::graphics
{
class GpuOptions;
class GpuAllocator;

class TextureStorage
{
    public:
        TextureStorage(GpuOptions* base, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        ~TextureStorage() noexcept;
        void UpdateBuffer(const TextureBufferData& textureBufferData);

    private:
        void LoadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadAllTextureBuffer();

        GpuOptions* m_base;
        GpuAllocator* m_allocator;
        std::vector<TextureBuffer> m_textureBuffers;
        vk::UniqueSampler m_sampler;
        nc::Connection<const TextureBufferData&> m_onTextureUpdate;
};
} // namespace nc::graphics