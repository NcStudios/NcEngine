#pragma once

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    struct Core;

    /** This class deals with rendering options that are dependent on properties of the GPU. */
    class GpuOptions
    {
        public:
            GpuOptions(Core* core);

            const vk::Device& GetDevice() const noexcept; /** @todo: Remove and update references in a separate PR */
            const vk::Format& GetDepthFormat() noexcept;
            vk::SampleCountFlagBits GetMaxSamplesCount();

        private:

            Core* m_core;
            vk::Format m_depthFormat;
            vk::SampleCountFlagBits m_samplesCount;
            bool m_samplesInitialized;
            bool m_depthInitialized;
            vk::PhysicalDeviceProperties m_gpuProperties;
    };
} // namespace nc::graphics