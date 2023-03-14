#pragma once

#include "utility/Signal.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
struct TextureBufferData;

namespace graphics
{
class GpuAllocator;
struct TextureBuffer;

class TextureStorage
{
    public:
        TextureStorage(vk::Device device, GpuAllocator* allocator, Signal<const TextureBufferData&>& onTextureUpdate);
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
} // namespace graphics
} // namespace nc
