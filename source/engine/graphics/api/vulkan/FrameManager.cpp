#include "FrameManager.h"

namespace
{
std::vector<nc::graphics::PerFrameGpuContext> CreatePerFrameGpuContextVector(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
    auto out = std::vector<nc::graphics::PerFrameGpuContext>{};
    out.reserve(nc::graphics::MaxFramesInFlight);
    std::generate_n(std::back_inserter(out), nc::graphics::MaxFramesInFlight, [logicalDevice, physicalDevice, surface](){ return nc::graphics::PerFrameGpuContext(logicalDevice, physicalDevice, surface); } );
    return out;
}
}

namespace nc::graphics
{
FrameManager::FrameManager(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
    : m_currentFrameIndex{0},
      m_perFrameGpuContext{CreatePerFrameGpuContextVector(logicalDevice, physicalDevice, surface)}
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
}