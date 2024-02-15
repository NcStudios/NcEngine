#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"

namespace nc::graphics::vulkan
{
class Image
{
    public:
        inline static uint32_t ImageUid = 0u;
    
        Image();
        Image(GpuAllocator* allocator, const unsigned char* pixels, uint32_t width, uint32_t height, bool isNormal);
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;
        Image& operator=(const Image&) = delete;
        Image(const Image&) = delete;

        auto GetImageView() noexcept -> vk::ImageView& { return m_view.get(); }
        void Clear() noexcept;

    private:
        GpuAllocation<vk::Image> m_image;
        vk::UniqueImageView m_view;
};

struct TextureArrayBuffer
{
    TextureArrayBuffer(vk::Device device);

    vk::UniqueSampler sampler;
    std::vector<Image> images;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    std::vector<uint32_t> uids;
};
} // namespace nc::graphics::vulkan
