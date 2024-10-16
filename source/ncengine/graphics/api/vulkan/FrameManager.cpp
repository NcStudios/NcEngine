#include "FrameManager.h"
#include "core/Device.h"

#include <algorithm>
#include <ranges>

namespace
{
auto CreatePerFrameGpuContextVector(const nc::graphics::vulkan::Device& device) -> std::vector<nc::graphics::vulkan::PerFrameGpuContext>
{
    auto out = std::vector<nc::graphics::vulkan::PerFrameGpuContext>{};
    out.reserve(nc::graphics::MaxFramesInFlight);
    std::generate_n(std::back_inserter(out), nc::graphics::MaxFramesInFlight, [&device, i = 0u]() mutable
    {
        return nc::graphics::vulkan::PerFrameGpuContext(device, i++);
    });

    return out;
}
} // anonymous namespace

namespace nc::graphics::vulkan
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

auto FrameManager::CommandBuffers() noexcept -> std::array<vk::CommandBuffer*, MaxFramesInFlight>
{
    auto cmdBuffers = std::array<vk::CommandBuffer*, MaxFramesInFlight>{};
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        cmdBuffers.at(i) = m_perFrameGpuContext.at(i).CommandBuffer();
    }
    return cmdBuffers;
}
} // namespace nc::graphics::vulkan
