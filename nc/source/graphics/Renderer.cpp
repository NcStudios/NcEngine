#include "Renderer.h"
#include "assets/AssetServices.h"
#include "debug/Profiler.h"
#include "ecs/component/DebugWidget.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/Transform.h"
#include "ecs/Registry.h"
#include "graphics/Commands.h"
#include "graphics/Graphics.h"
#include "graphics/Swapchain.h"
#include "graphics/techniques/PhongAndUiTechnique.h"
#include "graphics/techniques/ShadowMappingTechnique.h"
#include "PerFrameRenderState.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/techniques/WireframeTechnique.h"
#endif

#include <span>
#include <iostream>

namespace nc::graphics
{
    Renderer::Renderer(Graphics* graphics, 
                       ShaderResourceServices* shaderResources,
                       Vector2 dimensions)
        : m_graphics{graphics},
          m_shaderResources{shaderResources},
          m_renderPasses{std::make_unique<RenderPassManager>(graphics, dimensions)},
          m_dimensions{dimensions}
    {
        RegisterRenderPasses();
        RegisterTechniques();
    }

    Renderer::~Renderer() noexcept
    {
        m_renderPasses.reset();
    }
    
    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, AssetServices* assetServices, uint32_t currentSwapChainImageIndex)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto* cmd = BeginFrame(commands, assetServices, currentSwapChainImageIndex);

        /** Shadow mapping pass */
        m_renderPasses->Execute(RenderPassManager::ShadowMappingPass, cmd, 0u, state);

        /** Lit shading pass */
        m_renderPasses->Execute(RenderPassManager::LitShadingPass, cmd, currentSwapChainImageIndex, state);

        cmd->end();
        NC_PROFILE_END();
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
        auto& depthImageView = m_graphics->GetDepthStencil().GetImageView();
        uint32_t index = 0;
        for (auto& imageView : colorImageViews) { m_renderPasses->RegisterAttachments(std::vector<vk::ImageView>{imageView, depthImageView}, RenderPassManager::LitShadingPass, index++); }
    }

    void Renderer::RegisterTechniques()
    {
        m_renderPasses->RegisterTechnique<ShadowMappingTechnique>(RenderPassManager::ShadowMappingPass);

        #ifdef NC_EDITOR_ENABLED
        m_renderPasses->RegisterTechnique<WireframeTechnique>(RenderPassManager::LitShadingPass);
        #endif

        m_renderPasses->RegisterTechnique<PhongAndUiTechnique>(RenderPassManager::LitShadingPass);
    }

    vk::CommandBuffer* Renderer::BeginFrame(Commands* commands, AssetServices* assetServices, uint32_t currentSwapChainImageIndex)
    {
        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];
        cmd->begin(vk::CommandBufferBeginInfo{});

        SetViewportAndScissor(cmd, m_dimensions);

        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, assetServices->meshManager.GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*(assetServices->meshManager.GetIndexBuffer()), 0, vk::IndexType::eUint32);

        return cmd;
    }

    void Renderer::Clear() noexcept
    {
    }
}