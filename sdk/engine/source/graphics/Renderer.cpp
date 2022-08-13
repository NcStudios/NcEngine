#include "Renderer.h"

#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "graphics/DebugWidget.h"
#include "graphics/MeshRenderer.h"

#include "assets/AssetServices.h"
#include "graphics/Base.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"
#include "graphics/techniques/EnvironmentTechnique.h"
#include "graphics/techniques/ParticleTechnique.h"
#include "graphics/techniques/PbrTechnique.h"
#include "graphics/techniques/UiTechnique.h"
#include "graphics/techniques/ShadowMappingTechnique.h"
#include "graphics/vk/Swapchain.h"
#include "optick/optick.h"
#include "PerFrameRenderState.h"
#include "resources/RenderTarget.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/techniques/WireframeTechnique.h"
#endif

#include <span>

namespace
{
void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const nc::Vector2& dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    commandBuffer->setViewport(0, 1, &viewport);
    commandBuffer->setScissor(0, 1, &scissor);
}
}

namespace nc::graphics
{
    Renderer::Renderer(Graphics* graphics, ShaderResourceServices* shaderResources, Vector2 dimensions)
        : m_graphics{graphics},
          m_shaderResources{shaderResources},
          m_renderPasses{std::make_unique<RenderPassManager>(graphics, dimensions)},
          m_dimensions{dimensions},
          m_depthStencil{ std::make_unique<RenderTarget>(m_graphics->GetBasePtr(), m_graphics->GetAllocatorPtr(), m_dimensions, true, m_graphics->GetBasePtr()->GetMaxSamplesCount()) },
          m_colorBuffer{ std::make_unique<RenderTarget>(m_graphics->GetBasePtr(), m_graphics->GetAllocatorPtr(), m_dimensions, false, m_graphics->GetBasePtr()->GetMaxSamplesCount(), m_graphics->GetSwapchainPtr()->GetFormat()) }
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

    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, AssetServices* assetServices, const GpuAssetsStorage& gpuAssetsStorage, uint32_t currentSwapChainImageIndex)
    {
        OPTICK_CATEGORY("Renderer::Record", Optick::Category::Rendering);
        auto* cmd = BeginFrame(commands, assetServices, gpuAssetsStorage, currentSwapChainImageIndex);

        /** Shadow mapping pass */
        m_renderPasses->Execute(RenderPassManager::ShadowMappingPass, cmd, 0u, state);

        /** Lit shading pass */
        m_renderPasses->Execute(RenderPassManager::LitShadingPass, cmd, currentSwapChainImageIndex, state);

        cmd->end();
    }

    void Renderer::InitializeImgui()
    {
        m_graphics->GetBasePtr()->InitializeImgui(m_renderPasses->Acquire(RenderPassManager::LitShadingPass).renderpass.get());
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

    vk::CommandBuffer* Renderer::BeginFrame(Commands* commands, AssetServices* assetServices, const GpuAssetsStorage& gpuAssetsStorage, uint32_t currentSwapChainImageIndex)
    {
        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];
        cmd->begin(vk::CommandBufferBeginInfo{});

        SetViewportAndScissor(cmd, m_dimensions);

        vk::DeviceSize offsets[] = { 0 };
        const auto& vertexData = gpuAssetsStorage.GetVertexData();
        const auto& indexData = gpuAssetsStorage.GetIndexData();
        auto vertexBuffer = vertexData.buffer.GetBuffer();
        cmd->bindVertexBuffers(0, 1, &vertexBuffer, offsets);
        cmd->bindIndexBuffer(indexData.buffer.GetBuffer(), 0, vk::IndexType::eUint32);

        return cmd;
    }

    void Renderer::Clear() noexcept
    {
    }
}