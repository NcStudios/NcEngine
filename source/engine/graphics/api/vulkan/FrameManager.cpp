#include "FrameManager.h"
#include "core/Device.h"

#include <algorithm>

namespace
{
auto CreatePerFrameGpuContextVector(const nc::graphics::Device& device) -> std::vector<nc::graphics::PerFrameGpuContext>
{
    auto out = std::vector<nc::graphics::PerFrameGpuContext>{};
    out.reserve(nc::graphics::MaxFramesInFlight);
    std::generate_n(std::back_inserter(out), nc::graphics::MaxFramesInFlight, [&device]()
    {
        return nc::graphics::PerFrameGpuContext(device);
    });

    return out;
}
} // anonymous namespace

namespace nc::graphics
{
FrameManager::FrameManager(const Device& device)
    : m_perFrameGpuContext{::CreatePerFrameGpuContextVector(device)},
      m_currentFrameIndex{0}
{
}

void FrameManager::Begin()
{
    CurrentFrameContext()->WaitForSync();

    auto* cmd = CurrentFrameContext()->CommandBuffer();
    cmd->begin(vk::CommandBufferBeginInfo{});
}

void FrameManager::End()
{
    m_currentFrameIndex = (m_currentFrameIndex + 1) % MaxFramesInFlight;
}
} // namespace nc::graphics
