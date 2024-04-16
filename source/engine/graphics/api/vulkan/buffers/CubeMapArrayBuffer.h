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
        CubeMap(GpuAllocator* allocator, const asset::CubeMapWithId& data);
        ~CubeMap() noexcept;
        CubeMap(CubeMap&&) noexcept;
        CubeMap& operator=(CubeMap&&) noexcept;
        CubeMap& operator=(const CubeMap&) = delete;
        CubeMap(const CubeMap&) = delete;

        const vk::ImageView& GetImageView() const noexcept;
        void Clear() noexcept;

    private:
        GpuAllocation<vk::Image> m_image;
        vk::UniqueImageView m_cubeMapView;
};

struct CubeMapArrayBuffer
{
    CubeMapArrayBuffer(vk::Device device);

    vk::UniqueSampler sampler;
    std::vector<CubeMap> cubeMaps;
    std::vector<vk::DescriptorImageInfo> imageInfos;
    std::vector<std::string_view> uids;
};
} // namespace graphics::vulkan
} // namespace nc
