#pragma once

#include "graphics/api/vulkan/GpuAllocator.h"
#include "utility/Memory.h"

namespace nc { struct TaggedCubeMap; }

namespace nc::graphics
{
    class CubeMap
    {
        public:
            CubeMap(vk::Device device, GpuAllocator* allocator, const TaggedCubeMap& data);
            ~CubeMap() noexcept;
            CubeMap(CubeMap&&) noexcept;
            CubeMap& operator=(CubeMap&&) noexcept;
            CubeMap& operator=(const CubeMap&) = delete;
            CubeMap(const CubeMap&) = delete;

            const vk::ImageView& GetImageView() const noexcept;
            void Bind(const TaggedCubeMap& data);
            void Clear() noexcept;
            const std::string& GetUid() const noexcept;

        private:
            vk::Device m_device;
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Image> m_image;
            vk::UniqueImageView m_cubeMapView;
            std::string m_uid;
    };
}