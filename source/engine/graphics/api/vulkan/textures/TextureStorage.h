#pragma once

#include "utility/Signal.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
struct TextureUpdateEventData;

namespace graphics
{
class GpuAllocator;
struct TextureBuffer;

class TextureStorage
{
    public:
        TextureStorage(vk::Device device, GpuAllocator* allocator, Signal<const TextureUpdateEventData&>& onTextureUpdate);
        ~TextureStorage() noexcept;
        void UpdateBuffer(const TextureUpdateEventData& eventData);

    private:
        void LoadTextureBuffer(const TextureUpdateEventData& eventData);
        void UnloadTextureBuffer(const TextureUpdateEventData& eventData);
        void UnloadAllTextureBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<TextureBuffer> m_textureBuffers;
        vk::UniqueSampler m_sampler;
        nc::Connection<const TextureUpdateEventData&> m_onTextureUpdate;
};
} // namespace graphics
} // namespace nc
