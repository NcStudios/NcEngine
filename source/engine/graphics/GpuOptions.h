#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    /** This class deals with rendering options that are dependent on properties of the GPU (the physical device). */
    class GpuOptions
    {
        public:
            GpuOptions(vk::PhysicalDevice physicalDevice);

            const vk::Format& GetDepthFormat() noexcept;
            vk::SampleCountFlagBits GetMaxSamplesCount();

        private:

            vk::PhysicalDevice m_physicalDevice;
            vk::Format m_depthFormat;
            vk::SampleCountFlagBits m_samplesCount;
            bool m_samplesInitialized;
            bool m_depthInitialized;
            vk::PhysicalDeviceProperties m_gpuProperties;
    };
} // namespace nc::graphics