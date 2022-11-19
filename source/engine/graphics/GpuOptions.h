#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
/** This class deals with rendering options that are dependent on properties of the GPU (the physical device). */
class GpuOptions
{
    public:
        GpuOptions(vk::PhysicalDevice physicalDevice);

        vk::Format GetDepthFormat() const noexcept;
        vk::SampleCountFlagBits GetMaxSamplesCount() const noexcept;

    private:

        vk::PhysicalDevice m_physicalDevice;
        vk::Format m_depthFormat;
        vk::PhysicalDeviceProperties m_gpuProperties;
        vk::SampleCountFlagBits m_samplesCount;
};
} // namespace nc::graphics