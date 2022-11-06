#include "GpuOptions.h"
#include "config/Config.h"
#include "utility/NcError.h"

#include <string>

namespace
{
vk::Format QueryDepthFormatSupport(vk::PhysicalDevice physicalDevice)
{
    std::vector<vk::Format> depthFormats = { vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };

    for (auto& format : depthFormats)
    {
        vk::FormatProperties formatProperties;
        physicalDevice.getFormatProperties(format, &formatProperties);

        // Format must support depth stencil attachment for optimal tiling
        if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            return format;
        }
    }
    throw nc::NcError("Could not find a matching depth format");
}
}

namespace nc::graphics
{
GpuOptions::GpuOptions(vk::PhysicalDevice physicalDevice)
    : m_physicalDevice{physicalDevice},
      m_depthFormat{},
      m_samplesCount{},
      m_samplesInitialized{false},
      m_gpuProperties{physicalDevice.getProperties()}
{
}

vk::SampleCountFlagBits GpuOptions::GetMaxSamplesCount()
{
    if (m_samplesInitialized == true)
    {
        return m_samplesCount;
    }

    m_samplesInitialized = true;
    auto antialiasingSamples = nc::config::GetGraphicsSettings().antialiasing;
    vk::SampleCountFlags countsFromConfig = vk::SampleCountFlagBits::e1;

    if      (antialiasingSamples >= 64) countsFromConfig = vk::SampleCountFlagBits::e64;
    else if (antialiasingSamples >= 32) countsFromConfig = vk::SampleCountFlagBits::e32;
    else if (antialiasingSamples >= 16) countsFromConfig = vk::SampleCountFlagBits::e16;
    else if (antialiasingSamples >= 8)  countsFromConfig = vk::SampleCountFlagBits::e8;
    else if (antialiasingSamples >= 4)  countsFromConfig = vk::SampleCountFlagBits::e4;
    else if (antialiasingSamples >= 2)  countsFromConfig = vk::SampleCountFlagBits::e2;
    else countsFromConfig = vk::SampleCountFlagBits::e1;

    auto counts = m_gpuProperties.limits.framebufferColorSampleCounts & m_gpuProperties.limits.framebufferDepthSampleCounts;
    if (countsFromConfig < counts)
    {
        counts = countsFromConfig;
    }

    if      (counts & vk::SampleCountFlagBits::e64) m_samplesCount = vk::SampleCountFlagBits::e64;
    else if (counts & vk::SampleCountFlagBits::e32) m_samplesCount = vk::SampleCountFlagBits::e32;
    else if (counts & vk::SampleCountFlagBits::e16) m_samplesCount = vk::SampleCountFlagBits::e16;
    else if (counts & vk::SampleCountFlagBits::e8)  m_samplesCount = vk::SampleCountFlagBits::e8;
    else if (counts & vk::SampleCountFlagBits::e4)  m_samplesCount = vk::SampleCountFlagBits::e4;
    else if (counts & vk::SampleCountFlagBits::e2)  m_samplesCount = vk::SampleCountFlagBits::e2;
    else m_samplesCount = vk::SampleCountFlagBits::e1;

    return m_samplesCount;
}

const vk::Format& GpuOptions::GetDepthFormat() noexcept
{
    if (m_depthInitialized == true)
    {
        return m_depthFormat;
    }

    m_depthInitialized = true;
    m_depthFormat = QueryDepthFormatSupport(m_physicalDevice);
    return m_depthFormat;
}
}  // namespace nc::graphics
