#include "Imgui.h"
#include "core/Device.h"
#include "core/Instance.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include <filesystem>
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
    imguiDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
    imguiDescriptorPoolInfo.setMaxSets(1000);
    imguiDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(imguiPoolSizes.size()));
    imguiDescriptorPoolInfo.setPPoolSizes(imguiPoolSizes.data());

    return device.createDescriptorPoolUnique(imguiDescriptorPoolInfo);
}
} // anonymous namespace

namespace nc::graphics
{
Imgui::Imgui(const Device& device,
             const Instance& instance,
             GLFWwindow* window,
             vk::RenderPass renderPass)
    : m_imguiDescriptorPool{CreateImguiDescriptorPool(device.VkDevice())}
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);

    auto initInfo = ImGui_ImplVulkan_InitInfo{};
    initInfo.Instance = instance.VkInstance();
    initInfo.PhysicalDevice = device.VkPhysicalDevice();
    initInfo.Device = device.VkDevice();
    initInfo.Queue = device.VkGraphicsQueue();
    initInfo.DescriptorPool = m_imguiDescriptorPool.get();
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = VkSampleCountFlagBits(device.GetGpuOptions().GetMaxSamplesCount());

    ImGui_ImplVulkan_Init(&initInfo, renderPass);

    constexpr auto fontPath =  "assets\\fonts\\font.ttf";
    if (std::filesystem::exists(fontPath))
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("assets\\fonts\\font.ttf", 16.0f);
    }

    device.ExecuteCommand([](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
    ImGui_ImplVulkan_DestroyFontUploadObjects();

}

Imgui::~Imgui() noexcept
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Imgui::FrameBegin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Imgui::Frame()
{
    ImGui::Render();
}
} // namespace nc::graphics