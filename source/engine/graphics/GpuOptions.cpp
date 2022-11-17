#include "GpuOptions.h"
#include "config/Config.h"
#include "utility/NcError.h"

#include <string>
#include <algorithm>

namespace
{
vk::Format QueryDepthFormatSupport(vk::PhysicalDevice physicalDevice)
{
    constexpr auto depthFormats = std::array<vk::Format, 5>{ vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };

    auto formatPos = std::find_if(depthFormats.begin(), depthFormats.end(), [physicalDevice](auto&& format)
    {
        auto formatProperties = vk::FormatProperties{};
        physicalDevice.getFormatProperties(format, &formatProperties);

        // Format must support depth stencil attachment for optimal tiling
        return (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    });

    if (formatPos == depthFormats.end())
    {
        throw nc::NcError("Could not find a matching depth format");
    }
    return *formatPos;
}

vk::SampleCountFlagBits QueryMaxSamplesCount(vk::PhysicalDeviceProperties gpuProperties)
{
    auto antialiasingSamples = nc::config::GetGraphicsSettings().antialiasing;
    vk::SampleCountFlags countsFromConfig = vk::SampleCountFlagBits::e1;

    if      (antialiasingSamples >= 64) countsFromConfig = vk::SampleCountFlagBits::e64;
    else if (antialiasingSamples >= 32) countsFromConfig = vk::SampleCountFlagBits::e32;
    else if (antialiasingSamples >= 16) countsFromConfig = vk::SampleCountFlagBits::e16;
    else if (antialiasingSamples >= 8)  countsFromConfig = vk::SampleCountFlagBits::e8;
    else if (antialiasingSamples >= 4)  countsFromConfig = vk::SampleCountFlagBits::e4;
    else if (antialiasingSamples >= 2)  countsFromConfig = vk::SampleCountFlagBits::e2;
    else countsFromConfig = vk::SampleCountFlagBits::e1;

    auto counts = gpuProperties.limits.framebufferColorSampleCounts & gpuProperties.limits.framebufferDepthSampleCounts;
    if (countsFromConfig < counts)
    {
        counts = countsFromConfig;
    }

    vk::SampleCountFlagBits samplesCount;

    if      (counts & vk::SampleCountFlagBits::e64) samplesCount = vk::SampleCountFlagBits::e64;
    else if (counts & vk::SampleCountFlagBits::e32) samplesCount = vk::SampleCountFlagBits::e32;
    else if (counts & vk::SampleCountFlagBits::e16) samplesCount = vk::SampleCountFlagBits::e16;
    else if (counts & vk::SampleCountFlagBits::e8)  samplesCount = vk::SampleCountFlagBits::e8;
    else if (counts & vk::SampleCountFlagBits::e4)  samplesCount = vk::SampleCountFlagBits::e4;
    else if (counts & vk::SampleCountFlagBits::e2)  samplesCount = vk::SampleCountFlagBits::e2;
    else samplesCount = vk::SampleCountFlagBits::e1;

    return samplesCount;
}
}

namespace nc::graphics
{
GpuOptions::GpuOptions(vk::PhysicalDevice physicalDevice)
    : m_physicalDevice{physicalDevice},
      m_depthFormat{QueryDepthFormatSupport(m_physicalDevice)},
      m_gpuProperties{physicalDevice.getProperties()},
      m_samplesCount{QueryMaxSamplesCount(m_gpuProperties)}
{
}

vk::SampleCountFlagBits GpuOptions::GetMaxSamplesCount() const noexcept
{
    return m_samplesCount;
}

vk::Format GpuOptions::GetDepthFormat() const noexcept
{
    return m_depthFormat;
}
}  // namespace nc::graphics
