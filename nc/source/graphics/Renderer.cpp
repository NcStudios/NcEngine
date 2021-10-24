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
          m_dimensions{dimensions},
          m_phongAndUiTechnique{nullptr},
          m_shadowMappingTechnique{nullptr}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_wireframeTechnique{nullptr}
          #endif
    {
        RegisterRenderPasses();
        RegisterTechniques();
    }

    Renderer::~Renderer() noexcept
    {
        m_renderPasses.reset();
        m_phongAndUiTechnique.reset();
        m_shadowMappingTechnique.reset();
        #ifdef NC_EDITOR_ENABLED
        m_wireframeTechnique.reset();
        #endif
    }

    void Renderer::InitializeImgui()
    {
        m_graphics->GetBasePtr()->InitializeImgui(m_renderPasses->Acquire("Lit Pass").renderpass.get());
    }

    void Renderer::RegisterRenderPasses()
    {
        auto* swapchain = m_graphics->GetSwapchainPtr();

        /** Shadow mapping pass */
        {
            const auto& shadowDepthImageView = m_shaderResources->GetShadowMapManager().GetImageView();
            m_renderPasses->RegisterAttachment(shadowDepthImageView, "Shadow Mapping Pass");
        }

        /** Lit shading pass */
        {
            auto& colorImageViews = swapchain->GetColorImageViews();
            auto& depthImageView = m_graphics->GetDepthStencil().GetImageView();

            uint32_t index = 0;
            for (auto& imageView : colorImageViews) { m_renderPasses->RegisterAttachments(std::vector<vk::ImageView>{imageView, depthImageView}, "Lit Pass", index++); }
        }
    }

    void Renderer::RegisterTechniques()
    {
        /** Shadow mapping pass */
        {
            m_shadowMappingTechnique = std::make_unique<ShadowMappingTechnique>(m_graphics, &m_renderPasses->Acquire("Shadow Mapping Pass").renderpass.get());
        }

        /** Lit shading pass */
        {
            m_phongAndUiTechnique = std::make_unique<PhongAndUiTechnique>(m_graphics, &m_renderPasses->Acquire("Lit Pass").renderpass.get());

            #ifdef NC_EDITOR_ENABLED
            m_wireframeTechnique = std::make_unique<WireframeTechnique>(m_graphics, &m_renderPasses->Acquire("Lit Pass").renderpass.get());
            #endif
        }
    }

    void Renderer::Resize(Vector2 dimensions, vk::Extent2D extent)
    {
        m_dimensions = dimensions;
        RegisterRenderPasses();
        m_renderPasses->Resize(dimensions, extent);
        RegisterTechniques();
    }

    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];
        cmd->begin(vk::CommandBufferBeginInfo{});

        SetViewportAndScissor(cmd, m_dimensions);
        BindSharedData(cmd);

        /** Shadow mapping pass */
        {
            m_renderPasses->Begin("Shadow Mapping Pass", cmd, 0);
            m_shadowMappingTechnique->Bind(cmd);
            m_shadowMappingTechnique->Record(cmd, state.pointLightVPs, state.meshes);
            m_renderPasses->End(cmd);
        }

        /** Lit shading pass */
        {
            m_renderPasses->Begin("Lit Pass", cmd, currentSwapChainImageIndex);

            #ifdef NC_EDITOR_ENABLED
            if (m_wireframeTechnique->HasDebugWidget())
            {
                m_wireframeTechnique->Bind(cmd);
                m_wireframeTechnique->Record(cmd, state.camViewMatrix, state.projectionMatrix);
            }
            #endif

            m_phongAndUiTechnique->Bind(cmd);
            if(!state.meshes.empty())
            {
                m_phongAndUiTechnique->Record(cmd, state.cameraPosition, state.meshes);
            }

            RecordUi(cmd);

            #ifdef NC_EDITOR_ENABLED
            if (m_wireframeTechnique)
            {
                m_wireframeTechnique->ClearDebugWidget();
            }
            #endif

            m_renderPasses->End(cmd);
        }

        cmd->end();
        NC_PROFILE_END();
    }

    void Renderer::BindSharedData(vk::CommandBuffer* cmd)
    {
        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, m_assets->meshManager.GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*(m_assets->meshManager.GetIndexBuffer()), 0, vk::IndexType::eUint32);
    }

    #ifdef NC_EDITOR_ENABLED
    void Renderer::RegisterDebugWidget(nc::DebugWidget widget)
    {
        m_wireframeTechnique->RegisterDebugWidget(std::move(widget));
    }

    void Renderer::ClearDebugWidget()
    {
        if (m_wireframeTechnique)
        {
            m_wireframeTechnique->ClearDebugWidget();
        }
    }
    #endif

    void Renderer::RecordUi(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
        NC_PROFILE_END();
    }

    void Renderer::Clear() noexcept
    {
    }
}