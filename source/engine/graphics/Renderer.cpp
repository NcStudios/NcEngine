#include "Renderer.h"

#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "graphics/DebugWidget.h"
#include "graphics/MeshRenderer.h"

#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"
#include "graphics/techniques/EnvironmentTechnique.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/PbrTechnique.h"
#include "graphics/techniques/UiTechnique.h"
#include "graphics/techniques/ShadowMappingTechnique.h"
#include "graphics/vk/Swapchain.h"
#include "imgui/imgui_impl_vulkan.h"
#include "optick/optick.h"
#include "PerFrameRenderState.h"
#include "resources/RenderTarget.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"
#include "vk/Engine.h"
#include "vk/PerFrameGpuContext.h"

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
Renderer::Renderer(Graphics* graphics, vk::Device device, ShaderResourceServices* shaderResources, Vector2 dimensions)
    : m_graphics{graphics},
      m_shaderResources{shaderResources},
      m_renderPasses{std::make_unique<RenderPassManager>(graphics, dimensions)},
      m_dimensions{dimensions},
      m_depthStencil{ std::make_unique<RenderTarget>(device, m_graphics->GetAllocatorPtr(), m_dimensions, true, m_graphics->GetBasePtr()->GetMaxSamplesCount(), m_graphics->GetBasePtr()->GetDepthFormat()) },
      m_colorBuffer{ std::make_unique<RenderTarget>(device, m_graphics->GetAllocatorPtr(), m_dimensions, false, m_graphics->GetBasePtr()->GetMaxSamplesCount(), m_graphics->GetSwapchainPtr()->GetFormat()) },
      m_imguiDescriptorPool{CreateImguiDescriptorPool(device)}
{
    RegisterRenderPasses();
    RegisterTechniques();
}

Renderer::~Renderer() noexcept
{
    m_depthStencil.reset();
    m_colorBuffer.reset();
    m_renderPasses.reset();
}

void Renderer::Record(PerFrameGpuContext* currentFrame, const PerFrameRenderState& state, const MeshStorage& meshStorage, uint32_t currentSwapChainImageIndex)
{
    auto cmd = currentFrame->CommandBuffer();

    OPTICK_CATEGORY("Renderer::Record", Optick::Category::Rendering);
    SetViewportAndScissor(cmd, m_dimensions);
    BindMeshBuffers(cmd, meshStorage.GetVertexData(), meshStorage.GetIndexData());

    /** Shadow mapping pass */
    m_renderPasses->Execute(RenderPassManager::ShadowMappingPass, cmd, 0u, state);

    /** Lit shading pass */
    m_renderPasses->Execute(RenderPassManager::LitShadingPass, cmd, currentSwapChainImageIndex, state);

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

    ImGui_ImplVulkan_Init(&initInfo, m_renderPasses->Acquire(RenderPassManager::LitShadingPass).renderpass.get());
    commands->ExecuteCommand([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd);});
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Renderer::RegisterRenderPasses()
{
    auto* swapchain = m_graphics->GetSwapchainPtr();

    /** Shadow mapping pass */
    const auto& shadowDepthImageView = m_shaderResources->GetShadowMapManager().GetImageView();
    m_renderPasses->RegisterAttachment(shadowDepthImageView, RenderPassManager::ShadowMappingPass);

    /** Lit shading pass */
    auto& colorImageViews = swapchain->GetColorImageViews();
    auto& depthImageView = m_depthStencil->GetImageView();
    auto& colorResolveView = m_colorBuffer->GetImageView();
    uint32_t index = 0;

    for (auto& imageView : colorImageViews) { m_renderPasses->RegisterAttachments(std::vector<vk::ImageView>{colorResolveView, depthImageView, imageView}, RenderPassManager::LitShadingPass, index++); }
}

void Renderer::RegisterTechniques()
{
    m_renderPasses->RegisterTechnique<ShadowMappingTechnique>(RenderPassManager::ShadowMappingPass);

    #ifdef NC_EDITOR_ENABLED
    m_renderPasses->RegisterTechnique<WireframeTechnique>(RenderPassManager::LitShadingPass);
    #endif

    m_renderPasses->RegisterTechnique<EnvironmentTechnique>(RenderPassManager::LitShadingPass);
    m_renderPasses->RegisterTechnique<PbrTechnique>(RenderPassManager::LitShadingPass);
    m_renderPasses->RegisterTechnique<ParticleTechnique>(RenderPassManager::LitShadingPass);
    m_renderPasses->RegisterTechnique<UiTechnique>(RenderPassManager::LitShadingPass);
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
}
} // namespace nc::graphics