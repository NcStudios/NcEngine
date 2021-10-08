#include "Renderer.h"
#include "Ecs.h"
#include "component/DebugWidget.h"
#include "component/MeshRenderer.h"
#include "component/Transform.h"
#include "debug/Profiler.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "PerFrameRenderState.h"
#include "graphics/resources/ResourceManager.h"
#include "graphics/techniques/PhongAndUiTechnique.h"
#include "graphics/Swapchain.h"
#include "graphics/Initializers.h"

#include <span>

namespace nc::graphics
{
    Renderer::Renderer(graphics::Graphics* graphics)
        : m_graphics{graphics},
          m_textureManager{graphics},
          m_meshManager{graphics},
          m_mainRenderPass{m_graphics->GetSwapchainPtr()->GetPassDefinition()},
          m_phongAndUiTechnique{nullptr}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_wireframeTechnique{nullptr}
          #endif
          //m_particleTechnique{nullptr}
    {
    }

    Renderer::~Renderer() noexcept
    {
        m_graphics->GetBasePtr()->GetDevice().destroyRenderPass(m_mainRenderPass);
    }

    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex)
    {

        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        
        //@todo: these don't belong here.  
        if (m_phongAndUiTechnique == nullptr)
        {
            m_phongAndUiTechnique = std::make_unique<PhongAndUiTechnique>(m_graphics, &m_mainRenderPass);
        }

        #ifdef NC_EDITOR_ENABLED
        if (m_wireframeTechnique == nullptr)
        {
            m_wireframeTechnique = std::make_unique<WireframeTechnique>(m_graphics, &m_mainRenderPass);
        }
        #endif

        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];

        // Begin recording commands to each command buffer.
        cmd->begin(vk::CommandBufferBeginInfo{});
        BeginRenderPass(cmd, swapchain, &m_mainRenderPass, currentSwapChainImageIndex);
        BindSharedData(cmd);

        #ifdef NC_EDITOR_ENABLED
        if (m_wireframeTechnique->HasDebugWidget())
        {
            m_wireframeTechnique->Bind(cmd);
            m_wireframeTechnique->Record(cmd, state.viewMatrix, state.projectionMatrix);
        }
        #endif

        // if (m_particleTechnique)
        // {
        //     m_particleTechnique->Bind(cmd);
        //     m_particleTechnique->Record(cmd);
        // }

        m_phongAndUiTechnique->Bind(cmd);
        if(!state.meshes.empty())
        {
            m_phongAndUiTechnique->Record(cmd, state.cameraPosition, state.meshes);
        }

        RecordUi(cmd);

        // End recording commands to each command buffer.
        cmd->endRenderPass();
        cmd->end();

        #ifdef NC_EDITOR_ENABLED
        if (m_wireframeTechnique)
        {
            m_wireframeTechnique->ClearDebugWidget();
        }
        #endif

        NC_PROFILE_END();
    }

    void Renderer::BindSharedData(vk::CommandBuffer* cmd)
    {
        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, ResourceManager::GetVertexData().buffer.GetBuffer(), offsets);
        cmd->bindIndexBuffer(*ResourceManager::GetIndexData().buffer.GetBuffer(), 0, vk::IndexType::eUint32);
    }

    void Renderer::BeginRenderPass(vk::CommandBuffer* cmd, Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        auto dimensions = m_graphics->GetDimensions();

        vk::ClearValue clearValues[2];
		clearValues[0].setColor(vk::ClearColorValue(m_graphics->GetClearColor()));
		clearValues[1].setDepthStencil({ 1.0f, 0 });

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.setRenderPass(*renderPass); // Specify the render pass and attachments.
        renderPassInfo.setFramebuffer(swapchain->GetFrameBuffer(index));
        renderPassInfo.renderArea.setOffset({0,0}); // Specify the dimensions of the render area.
        renderPassInfo.renderArea.setExtent(swapchain->GetExtent());
        renderPassInfo.setClearValueCount(2); // Set clear color
        renderPassInfo.setPClearValues(clearValues);

        SetViewportAndScissor(cmd, dimensions);

        // Begin render pass and bind pipeline
        cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        NC_PROFILE_END();
    }

    // void Renderer::RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates)
    // {
    //     if (!m_particleTechnique)
    //     {
    //         m_particleTechnique = std::make_unique<ParticleTechnique>(m_graphics, &m_mainRenderPass);
    //     }
    //     m_particleTechnique->RegisterEmitters(m_emitterStates);
    // }

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

    // void Renderer::ClearParticleEmitters()
    // {
    //     if (m_particleTechnique)
    //     {
    //         m_particleTechnique->Clear();
    //     }
    // }

    void Renderer::Clear() noexcept
    {
    }
}