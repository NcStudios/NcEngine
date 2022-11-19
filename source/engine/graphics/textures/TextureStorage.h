#pragma once

#include "assets/AssetData.h"
#include "assets/AssetManagers.h"
#include "graphics/textures/Textures.h"

namespace nc::graphics
{
class GpuAllocator;

class TextureStorage
{
    public:
        TextureStorage(vk::Device device, GpuAllocator* allocator, const nc::GpuAccessorSignals& gpuAccessorSignals);
        ~TextureStorage() noexcept;
        void UpdateBuffer(const TextureBufferData& textureBufferData);

    private:
        void LoadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadTextureBuffer(const TextureBufferData& textureBufferData);
        void UnloadAllTextureBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<TextureBuffer> m_textureBuffers;
        vk::UniqueSampler m_sampler;
        nc::Connection<const TextureBufferData&> m_onTextureUpdate;
};
} // namespace nc::graphics