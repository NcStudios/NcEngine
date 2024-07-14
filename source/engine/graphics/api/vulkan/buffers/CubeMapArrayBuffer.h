#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"
#include "utility/Memory.h"

namespace nc
{
namespace asset
{
struct CubeMapWithId;
} // namespace asset

namespace graphics::vulkan
{
class CubeMap
{
    public:
        explicit CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data, vk::Format format, vk::ImageUsageFlags usage);
        CubeMap(const CubeMap&) = delete;
        CubeMap& operator=(const CubeMap&) = delete;
        CubeMap(CubeMap&&) = default;
        CubeMap& operator=(CubeMap&&) = default;
        ~CubeMap() noexcept;

        auto GetImageView() const noexcept -> const vk::ImageView&
        {
            return m_cubeMapView.get();
        }

        auto GetFaceViews() const noexcept -> std::vector<vk::ImageView>;

        void Clear() noexcept;

    private:
        GpuAllocation<vk::Image> m_image;
        vk::UniqueImageView m_cubeMapView;
        std::vector<vk::UniqueImageView> m_faceViews;
};

struct CubeMapArrayBuffer
{
    explicit CubeMapArrayBuffer(vk::Device device);

    vk::UniqueSampler sampler;
    std::vector<CubeMap> cubeMaps;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    std::vector<size_t> uids;
};
} // namespace graphics::vulkan
} // namespace nc
