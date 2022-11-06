#pragma once

#include "graphics/Commands.h"
#include "graphics/GpuAllocator.h"
#include "utility/Memory.h"

namespace nc::graphics
{
    class CubeMap
    {
        public:
            CubeMap(vk::Device device, GpuAllocator* allocator, const std::array<unique_c_ptr<unsigned char[]>, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize, const std::string& uid);
            ~CubeMap() noexcept;
            CubeMap(CubeMap&&) noexcept;
            CubeMap& operator=(CubeMap&&) noexcept;
            CubeMap& operator=(const CubeMap&) = delete;
            CubeMap(const CubeMap&) = delete;

            const vk::ImageView& GetImageView() const noexcept;
            void Bind(const std::array<unique_c_ptr<unsigned char[]>, 6>& pixels, uint32_t width, uint32_t height, uint32_t cubeMapSize);
            void Clear() noexcept;
            const std::string& GetUid() const noexcept;

        private:
            vk::Device m_device;
            GpuAllocator* m_allocator;
            GpuAllocation<vk::Image> m_image;
            vk::UniqueImageView m_cubeMapview;
            std::string m_uid;
    };
}