#pragma once

#include "PerFrameGpuContext.h"

#include <vector>

namespace nc::graphics
{
class Device;

class FrameManager
{
    public:
        FrameManager(const Device& device);

        void Begin();
        void End();

        auto Index() const noexcept -> uint32_t
        {
            return m_currentFrameIndex;
        };

        auto CurrentFrameContext() noexcept -> PerFrameGpuContext*
        {
            return &(m_perFrameGpuContext[m_currentFrameIndex]);
        }

    private:
        std::vector<PerFrameGpuContext> m_perFrameGpuContext;
        uint32_t m_currentFrameIndex; // Used to select which PerFrameGpuContext to use. Each frame in MaxFramesInFlight requires its own PerFrameGpuContext.
};
} // namespace nc::graphics
