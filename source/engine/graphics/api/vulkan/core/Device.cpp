#include "Device.h"
#include "Instance.h"
#include "detail/PhysicalDevice.h"

#include "ncutility/NcError.h"

#include <algorithm>

namespace
{
auto CreateLogicalDevice(vk::PhysicalDevice physicalDevice,
                         const nc::graphics::QueueFamilyIndices& indices,
                         std::span<const char* const> enabledExtensions) -> vk::UniqueDevice
{
    const auto uniqueQueueFamilies = indices.GetUniqueIndices();
    const auto queuePriority = std::array<float, 1>{1.0f};
    auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>{};
    std::ranges::transform(uniqueQueueFamilies, std::back_inserter(queueCreateInfos), [&queuePriority](auto queueFamily) 
    {
        return vk::DeviceQueueCreateInfo{
            {},
            queueFamily,
            1,
            queuePriority.data()
        };
    });

    auto deviceFeatures = vk::PhysicalDeviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(VK_TRUE);
    deviceFeatures.setFillModeNonSolid(VK_TRUE);
    deviceFeatures.setWideLines(VK_TRUE);
    deviceFeatures.setSampleRateShading(VK_TRUE);

    auto deviceCreateInfo = vk::DeviceCreateInfo{};
    deviceCreateInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()));
    deviceCreateInfo.setPQueueCreateInfos(queueCreateInfos.data());
    deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);
    deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()));
    deviceCreateInfo.setPpEnabledExtensionNames(enabledExtensions.data());
    deviceCreateInfo.setEnabledLayerCount(0);

    return physicalDevice.createDeviceUnique(deviceCreateInfo);
}
} // anonymous namespace

namespace nc::graphics
{
auto Device::Create(const Instance& instance, vk::SurfaceKHR surface,
                    std::span<const char* const> requiredExtensions) -> std::unique_ptr<Device>
{
    const auto [physicalDevice, queueIndices] = detail::SelectPhysicalDevice(
        instance.GetPhysicalDevices(), surface, requiredExtensions
    );

    return std::make_unique<Device>(physicalDevice, queueIndices, requiredExtensions);
}

Device::Device(vk::PhysicalDevice physicalDevice,
               const QueueFamilyIndices& queueIndices,
               std::span<const char* const> enabledExtensions)
    : m_physicalDevice{physicalDevice},
      m_device{::CreateLogicalDevice(physicalDevice, queueIndices, enabledExtensions)},
      m_queueIndices{queueIndices},
      m_graphicsQueue{m_device->getQueue(m_queueIndices.GraphicsFamilyIndex(), 0)},
      m_presentQueue{m_device->getQueue(m_queueIndices.PresentFamilyIndex(), 0)},
      m_gpuOptions{m_physicalDevice}
{
}

void Device::ExecuteCommand(std::function<void(vk::CommandBuffer)>&& function) const
{
    auto poolInfo = vk::CommandPoolCreateInfo{};
    poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    poolInfo.setQueueFamilyIndex(m_queueIndices.GraphicsFamilyIndex());

    auto commandPool = m_device->createCommandPoolUnique(poolInfo);

    auto allocInfo = vk::CommandBufferAllocateInfo{};
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandPool(commandPool.get());
    allocInfo.setCommandBufferCount(1);

    auto tempCommandBuffers = m_device->allocateCommandBuffers(allocInfo);
    auto tempCommandBuffer = tempCommandBuffers.at(0);

    auto beginInfo = vk::CommandBufferBeginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    tempCommandBuffer.begin(beginInfo);
    {
        function(tempCommandBuffer);
    }
    tempCommandBuffer.end();

    auto submitInfo = vk::SubmitInfo{};
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&tempCommandBuffer);
    m_graphicsQueue.submit(submitInfo, nullptr);
    m_graphicsQueue.waitIdle();
    m_device->freeCommandBuffers(commandPool.get(), tempCommandBuffer);
}
} // namespace graphics
