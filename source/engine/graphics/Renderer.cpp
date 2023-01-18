#include "Renderer.h"

#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "graphics/DebugWidget.h"
#include "graphics/MeshRenderer.h"

#include "graphics/GpuOptions.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"
#include "graphics/PointLight.h"
#include "graphics/renderpasses/RenderPassManager.h"
#include "graphics/renderpasses/RenderTarget.h"
#include "graphics/shaders/ShaderResources.h"
#include "graphics/techniques/EnvironmentTechnique.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/PbrTechnique.h"
#include "graphics/techniques/UiTechnique.h"
#include "graphics/techniques/ShadowMappingTechnique.h"
#include "graphics/Swapchain.h"
#include "imgui/imgui_impl_vulkan.h"
#include "optick/optick.h"
#include "PerFrameRenderState.h"
#include "Core.h"
#include "PerFrameGpuContext.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/techniques/WireframeTechnique.h"
#endif

#include <span>

namespace
{
/** @todo: Move this to a different place */
vk::UniqueDescriptorPool CreateImguiDescriptorPool(vk::Device device)
{
    // Create descriptor pool for IMGUI
    std::array<vk::DescriptorPoolSize, 11> imguiPoolSizes =
    {
        vk::DescriptorPoolSize { vk::DescriptorType::eSampler, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageImage, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformTexelBuffer, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageTexelBuffer, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformBufferDynamic, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBufferDynamic, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eInputAttachment, 1000 }
    };

    vk::DescriptorPoolCreateInfo imguiDescriptorPoolInfo = {};
    imguiDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    imguiDescriptorPoolInfo.setMaxSets(1000);
    imguiDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(imguiPoolSizes.size()));
    imguiDescriptorPoolInfo.setPPoolSizes(imguiPoolSizes.data());

    return device.createDescriptorPoolUnique(imguiDescriptorPoolInfo);
}

void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const nc::Vector2& dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    commandBuffer->setViewport(0, 1, &viewport);
    commandBuffer->setScissor(0, 1, &scissor);
}
} // anonymous namespace

namespace nc::graphics
{
inline static const std::string LitShadingPass = "Lit Pass";
inline static const std::string ShadowMappingPass = "Shadow Mapping Pass";

Renderer::Renderer(vk::Device device, Registry* registry, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderResources* shaderResources, Vector2 dimensions)
    : m_device{device},
      m_swapchain{swapchain},
      m_gpuOptions{gpuOptions},
      m_shaderResources{shaderResources},
      m_renderPasses{std::make_unique<RenderPassManager>(device, m_swapchain, m_gpuOptions, shaderResources->GetDescriptorSets(), dimensions)},
      m_dimensions{dimensions},
      m_depthStencil{ std::make_unique<RenderTarget>(device, gpuAllocator, m_dimensions, true, m_gpuOptions->GetMaxSamplesCount(), m_gpuOptions->GetDepthFormat()) },
      m_colorBuffer{ std::make_unique<RenderTarget>(device, gpuAllocator, m_dimensions, false, m_gpuOptions->GetMaxSamplesCount(), m_swapchain->GetFormat()) },
      m_imguiDescriptorPool{CreateImguiDescriptorPool(device)},
      m_onAddPointLightConnection{registry->OnAdd<PointLight>().Connect([this](graphics::PointLight&){ this->AddShadowMappingPass();}, 0u)},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){ this->RemoveShadowMappingPass();}, 0u)},
      m_numShadowCasters{0}
{
        /** Lit shading pass */
    std::array<AttachmentSlot, 3> litAttachmentSlots
    {
        AttachmentSlot{0, AttachmentType::Color, m_swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, m_gpuOptions->GetMaxSamplesCount()},
        AttachmentSlot{1, AttachmentType::Depth, m_gpuOptions->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, m_gpuOptions->GetMaxSamplesCount()},
        AttachmentSlot{2, AttachmentType::Resolve, m_swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    std::array<Subpass, 1> litSubpasses
    {
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
    };

    m_renderPasses->Add(LitShadingPass, litAttachmentSlots, litSubpasses, ClearValueFlags::Depth | ClearValueFlags::Color, m_dimensions);

    auto& colorImageViews = m_swapchain->GetColorImageViews();
    auto& depthImageView = m_depthStencil->GetImageView();
    auto& colorResolveView = m_colorBuffer->GetImageView();

    uint32_t index = 0;
    for (auto& imageView : colorImageViews) 
    {
        m_renderPasses->RegisterAttachments(std::vector<vk::ImageView>{colorResolveView, depthImageView, imageView}, LitShadingPass, index++); 
    }

    #ifdef NC_EDITOR_ENABLED
    m_renderPasses->RegisterTechnique<WireframeTechnique>(LitShadingPass);
    #endif

    m_renderPasses->RegisterTechnique<EnvironmentTechnique>(LitShadingPass);
    m_renderPasses->RegisterTechnique<PbrTechnique>(LitShadingPass);
    m_renderPasses->RegisterTechnique<ParticleTechnique>(LitShadingPass);
    m_renderPasses->RegisterTechnique<UiTechnique>(LitShadingPass);
}

Renderer::~Renderer() noexcept
{
    m_depthStencil.reset();
    m_colorBuffer.reset();
    m_renderPasses.reset();
}

void Renderer::Record(PerFrameGpuContext* currentFrame, const PerFrameRenderState& state, const MeshStorage& meshStorage, uint32_t currentSwapChainImageIndex)
{
    OPTICK_CATEGORY("Renderer::Record", Optick::Category::Rendering);
    auto cmd = currentFrame->CommandBuffer();

    SetViewportAndScissor(cmd, m_dimensions);
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    /** Shadow mapping pass */
    for (auto i = 0u; i < m_numShadowCasters; ++i)
    {
        m_renderPasses->Execute(ShadowMappingPass + std::to_string(i), cmd, 0u, state);
    }

    /** Lit shading pass */
    m_renderPasses->Execute(LitShadingPass, cmd, currentSwapChainImageIndex, state);

    cmd->end();
}

void Renderer::InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, Commands* commands, uint32_t maxSamplesCount)
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

    ImGui_ImplVulkan_Init(&initInfo, m_renderPasses->Acquire(LitShadingPass).renderpass.get());
    commands->ExecuteCommand([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Renderer::AddShadowMappingPass()
{
    auto id = ShadowMappingPass + std::to_string(m_numShadowCasters);

    std::array<AttachmentSlot, 1> shadowAttachmentSlots
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    std::array<Subpass, 1> shadowSubpasses
    {
        Subpass{shadowAttachmentSlots[0]}
    };

    m_renderPasses->Add(id, shadowAttachmentSlots, shadowSubpasses, ClearValueFlags::Depth, m_dimensions);
    m_renderPasses->RegisterAttachment(m_shaderResources->GetShadowMapShaderResource().GetImageView(m_numShadowCasters), id);

    m_renderPasses->UnregisterTechnique<ShadowMappingTechnique>(id);
    auto& renderpass = m_renderPasses->Acquire(id);
    renderpass.techniques.push_back(std::make_unique<ShadowMappingTechnique>(m_device, m_gpuOptions, m_shaderResources->GetDescriptorSets(), &renderpass.renderpass.get(), m_numShadowCasters));
    m_numShadowCasters++;

}

void Renderer::RemoveShadowMappingPass()
{
    auto id = ShadowMappingPass + std::to_string(m_numShadowCasters-1);
    m_renderPasses->UnregisterTechnique<ShadowMappingTechnique>(id);
    m_renderPasses->Remove(id);
    m_numShadowCasters--;
}

void Renderer::BindMeshBuffers(vk::CommandBuffer* cmd, const VertexBuffer& vertexData, const IndexBuffer& indexData)
{
    vk::DeviceSize offsets[] = { 0 };
    auto vertexBuffer = vertexData.buffer.GetBuffer();
    cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
    cmd->bindIndexBuffer(indexData.buffer.GetBuffer(), 0, vk::IndexType::eUint32);
}

void Renderer::Clear() noexcept
{
    auto itemsToRemove = m_numShadowCasters;
    for (uint32_t i = 0; i < itemsToRemove; ++i)
    {
        RemoveShadowMappingPass();
    }
}
} // namespace nc::graphics