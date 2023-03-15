#pragma once

#include "utility/Signal.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace nc
{
namespace asset
{
struct TextureUpdateEventData;
} // namespace asset

namespace graphics
{
class GpuAllocator;
struct TextureBuffer;

class TextureStorage
{
    public:
        TextureStorage(vk::Device device, GpuAllocator* allocator, Signal<const asset::TextureUpdateEventData&>& onTextureUpdate);
        ~TextureStorage() noexcept;

        void UpdateBuffer(const asset::TextureUpdateEventData& eventData);

    private:
        void LoadTextureBuffer(const asset::TextureUpdateEventData& eventData);
        void UnloadTextureBuffer(const asset::TextureUpdateEventData& eventData);
        void UnloadAllTextureBuffer();

        vk::Device m_device;
        GpuAllocator* m_allocator;
        std::vector<TextureBuffer> m_textureBuffers;
        vk::UniqueSampler m_sampler;
        nc::Connection<const asset::TextureUpdateEventData&> m_onTextureUpdate;
};
} // namespace graphics
} // namespace nc
