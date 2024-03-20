#pragma once

#include "graphics/api/vulkan/core/DeviceRequirements.h"

#include <vulkan/vulkan.hpp>

namespace nc::graphics::vulkan
{
/** This class deals with rendering options that are dependent on properties of the GPU (the physical device). */
class GpuOptions
{
    public:
        GpuOptions(vk::PhysicalDevice physicalDevice);

        auto GetDepthFormat() const noexcept -> vk::Format
        {
            return m_depthFormat;
        }

        auto GetMaxSamplesCount() const noexcept -> vk::SampleCountFlagBits
        {
            return m_samplesCount;
        }

        auto GetDeviceRequirements() const noexcept -> const DeviceRequirements*
        {
            return &m_deviceRequirements;
        }

    private:
        vk::Format m_depthFormat;
        vk::SampleCountFlagBits m_samplesCount;
        DeviceRequirements m_deviceRequirements;
};
} // namespace nc::graphics::vulkan
