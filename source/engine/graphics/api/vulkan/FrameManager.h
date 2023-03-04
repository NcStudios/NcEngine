#pragma once

#include "PerFrameGpuContext.h"

#include <memory>
#include <vector>

namespace nc::graphics
{
/** How many frames can be rendered concurrently. */
constexpr uint32_t MaxFramesInFlight = 2u;

class FrameManager
{
    public:
        FrameManager(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

        void Begin();
        void End();
        uint32_t Index() const noexcept { return m_currentFrameIndex; };
        PerFrameGpuContext* CurrentFrameContext() noexcept { return &(m_perFrameGpuContext[m_currentFrameIndex]); }

    private:
        uint32_t m_currentFrameIndex; // Used to select which PerFrameGpuContext to use. Each frame in MaxFramesInFlight requires its own PerFrameGpuContext.
        std::vector<PerFrameGpuContext> m_perFrameGpuContext;
};
}