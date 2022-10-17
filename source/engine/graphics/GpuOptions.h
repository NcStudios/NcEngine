#pragma once

#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    struct Core;

    class GpuOptions
    {
        public:
            GpuOptions(Core* core);

            const vk::Device& GetDevice() const noexcept; /** @todo: Remove and update references in a separate PR */
            const vk::PhysicalDevice& GetPhysicalDevice() const noexcept; /** @todo: Remove and update references in a separate PR */
            const vk::Instance& GetInstance() const noexcept; /** @todo: Remove and update references in a separate PR */
            const vk::SurfaceKHR& GetSurface() const noexcept; /** @todo: Remove and update references in a separate PR */
            const vk::Format& GetDepthFormat() const noexcept;
            vk::SampleCountFlagBits GetMaxSamplesCount();

            void QueryDepthFormatSupport();

        private:

            Core* m_core;
            vk::Format m_depthFormat;
            vk::SampleCountFlagBits m_samplesCount;
            bool m_samplesInitialized;

            vk::PhysicalDeviceProperties m_gpuProperties;
    };
} // namespace nc::graphics