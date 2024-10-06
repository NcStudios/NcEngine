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
        explicit CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data);

        auto GetImageView() const noexcept -> const vk::ImageView&
        {
            return m_cubeMapView.get();
        }

        void Clear() noexcept;

    private:
        GpuAllocation<vk::Image> m_image;
        vk::UniqueImageView m_cubeMapView;
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
