#include "Base.h"
#include "config/Config.h"
#include "graphics/Commands.h"
#include "utility/NcError.h"
#include "vk/Engine.h"
#include "vk/Initializers.h"

#include <algorithm>
#include <array>
#include <string>

namespace nc::graphics
{
    Base::Base(Engine* engine)
        : m_engine{engine},
          m_depthFormat{},
          m_samplesCount{},
          m_samplesInitialized{false},
          m_gpuProperties{m_engine->physicalDevice.getProperties()}
    {
        QueryDepthFormatSupport();
    }

    vk::SampleCountFlagBits Base::GetMaxSamplesCount()
    {
        if (m_samplesInitialized == true)
        {
            return m_samplesCount;
        }

        m_samplesInitialized = true;

        vk::PhysicalDeviceProperties properties{};
        m_engine->physicalDevice.getProperties(&properties);

        auto antialiasingSamples = nc::config::GetGraphicsSettings().antialiasing;
        vk::SampleCountFlags countsFromConfig = vk::SampleCountFlagBits::e1;

        if      (antialiasingSamples >= 64) countsFromConfig = vk::SampleCountFlagBits::e64;
        else if (antialiasingSamples >= 32) countsFromConfig = vk::SampleCountFlagBits::e32;
        else if (antialiasingSamples >= 16) countsFromConfig = vk::SampleCountFlagBits::e16;
        else if (antialiasingSamples >= 8)  countsFromConfig = vk::SampleCountFlagBits::e8;
        else if (antialiasingSamples >= 4)  countsFromConfig = vk::SampleCountFlagBits::e4;
        else if (antialiasingSamples >= 2)  countsFromConfig = vk::SampleCountFlagBits::e2;
        else countsFromConfig = vk::SampleCountFlagBits::e1;

        auto counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
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

    const vk::Device& Base::GetDevice() const noexcept /** @todo: Remove and update references in a separate PR */
    {
        return m_engine->logicalDevice.get();
    }

    const vk::PhysicalDevice& Base::GetPhysicalDevice() const noexcept /** @todo: Remove and update references in a separate PR */
    {
        return m_engine->physicalDevice;
    }

    const vk::Instance& Base::GetInstance() const noexcept /** @todo: Remove and update references in a separate PR */
    {
        return m_engine->instance.get();
    }

    const vk::SurfaceKHR& Base::GetSurface() const noexcept /** @todo: Remove and update references in a separate PR */
    {
        return m_engine->surface.get();
    }

    const vk::Format& Base::GetDepthFormat() const noexcept
    {
        return m_depthFormat;
    }

    void Base::QueryDepthFormatSupport()
    {
        std::vector<vk::Format> depthFormats = { vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };

        for (auto& format : depthFormats)
        {
            vk::FormatProperties formatProperties;
            m_engine->physicalDevice.getFormatProperties(format, &formatProperties);
            // Format must support depth stencil attachment for optimal tiling
            if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            {
                m_depthFormat = format;
                return;
            }
        }
        throw NcError("Could not find a matching depth format");
    }
}  // namespace nc::graphics
