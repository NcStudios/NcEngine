#include "Imgui.h"
#include "graphics/PerFrameRenderState.h"

#include "imgui/imgui_impl_vulkan.h"

namespace
{
auto CreateImguiDescriptorPool(vk::Device device) -> vk::UniqueDescriptorPool
{
    // Create descriptor pool for IMGUI
    const std::array<vk::DescriptorPoolSize, 11> imguiPoolSizes =
    {
        vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
        vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000}
    };

    vk::DescriptorPoolCreateInfo imguiDescriptorPoolInfo = {};
    imguiDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    imguiDescriptorPoolInfo.setMaxSets(1000);
    imguiDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(imguiPoolSizes.size()));
    imguiDescriptorPoolInfo.setPPoolSizes(imguiPoolSizes.data());

    return device.createDescriptorPoolUnique(imguiDescriptorPoolInfo);
}
} // anonymous namespace

namespace nc::graphics
{
Imgui::Imgui(vk::Device device)
    : m_imguiDescriptorPool{CreateImguiDescriptorPool(device)}
{
}

void Imgui::InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, vk::RenderPass renderPass, Commands* commands, uint32_t maxSamplesCount)
{
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = instance;
    initInfo.PhysicalDevice = physicalDevice;
    initInfo.Device = logicalDevice;
    initInfo.Queue = commands->GetCommandQueue(QueueFamilyType::GraphicsFamily);
    initInfo.DescriptorPool = m_imguiDescriptorPool.get();
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = VkSampleCountFlagBits(maxSamplesCount);

    ImGui_ImplVulkan_Init(&initInfo, renderPass);
    commands->ExecuteCommand([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}
} // namespace nc::graphics