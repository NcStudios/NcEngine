#include "Renderer.h"
#include "Ecs.h"
#include "component/DebugWidget.h"
#include "component/MeshRenderer.h"
#include "component/Transform.h"
#include "debug/Profiler.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "PerFrameRenderState.h"
#include "graphics/techniques/PhongAndUiTechnique.h"
#include "graphics/Swapchain.h"
#include "PerFrameRenderState.h"
#include "assets/AssetServices.h"
#include "resources/ShaderResourceServices.h"
#include "resources/RenderPassManager.h"

#include <span>
#include <iostream>

namespace nc::graphics
{
    Renderer::Renderer(Graphics* graphics, 
                       AssetServices* assets, 
                       ShaderResourceServices* shaderResources,
                       Vector2 dimensions)
        : m_graphics{graphics},
          m_assets{assets},
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
    
    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto* cmd = BeginFrame(commands, currentSwapChainImageIndex);

        /** Shadow mapping pass */
        m_renderPasses->Execute("Shadow Mapping Pass", cmd, 0u, state);

        /** Lit shading pass */
        m_renderPasses->Execute("Lit Pass", cmd, currentSwapChainImageIndex, state);

        cmd->end();
        NC_PROFILE_END();
    }

    void Renderer::InitializeImgui()
    {
        m_graphics->GetBasePtr()->InitializeImgui(m_renderPasses->Acquire("Lit Pass").renderpass.get());
    }

    void Renderer::RegisterRenderPasses()
    {
        auto* swapchain = m_graphics->GetSwapchainPtr();

        /** Shadow mapping pass */
        const auto& shadowDepthImageView = m_shaderResources->GetShadowMapManager().GetImageView();
        m_renderPasses->RegisterAttachment(shadowDepthImageView, "Shadow Mapping Pass");

        /** Lit shading pass */
        auto& colorImageViews = swapchain->GetColorImageViews();
        auto& depthImageView = m_graphics->GetDepthStencil().GetImageView();
        uint32_t index = 0;
        for (auto& imageView : colorImageViews) { m_renderPasses->RegisterAttachments(std::vector<vk::ImageView>{imageView, depthImageView}, "Lit Pass", index++); }
    }

    void Renderer::RegisterTechniques()
    {
        m_renderPasses->RegisterTechnique<ShadowMappingTechnique>("Shadow Mapping Pass");

        #ifdef NC_EDITOR_ENABLED
        m_renderPasses->RegisterTechnique<WireframeTechnique>("Lit Pass");
        #endif

        m_renderPasses->RegisterTechnique<PhongAndUiTechnique>("Lit Pass");
    }

    vk::CommandBuffer* Renderer::BeginFrame(Commands* commands, uint32_t currentSwapChainImageIndex)
    {
        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];
        cmd->begin(vk::CommandBufferBeginInfo{});

        SetViewportAndScissor(cmd, m_dimensions);

        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, m_assets->meshManager.GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*(m_assets->meshManager.GetIndexBuffer()), 0, vk::IndexType::eUint32);

        return cmd;
    }

    void Renderer::Clear() noexcept
    {
    }
}