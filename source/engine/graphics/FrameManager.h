#pragma once

#include "vk/PerFrameGpuContext.h"

#include <memory>
#include <vector>

namespace nc::graphics
{
/** How many frames can be rendered concurrently. */
constexpr uint32_t MaxFramesInFlight = 2u;

class PerFrameGpuContext;
class FrameManager
{
    public:
        FrameManager(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

        void Begin();
        void End();
        uint32_t Index() const noexcept { return m_currentFrameIndex; };
        PerFrameGpuContext* CurrentFrameContext() const noexcept { return m_perFrameGpuContext[m_currentFrameIndex].get(); }

    private:
        uint32_t m_currentFrameIndex; // Used to select which PerFrameGpuContext to use. Each frame in MaxFramesInFlight requires its own PerFrameGpuContext.
        std::vector<std::unique_ptr<PerFrameGpuContext>> m_perFrameGpuContext;
};
}