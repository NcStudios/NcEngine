#include "Renderer.h"

#include "component/vulkan/MeshRenderer.h"
#include "component/Transform.h"
#include "debug/Profiler.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/vulkan/techniques/PhongAndUiTechnique.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/Initializers.h"

namespace nc::graphics::vulkan
{
    Renderer::Renderer(graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_textureManager{graphics},
      m_meshManager{graphics},
      m_mainRenderPass{},
      m_phongAndUiTechnique{nullptr},
      m_wireframeTechnique{nullptr},
      m_particleTechnique{nullptr}
    {
        m_mainRenderPass = m_graphics->GetSwapchainPtr()->GetPassDefinition();
    }

    Renderer::~Renderer()
    {
        m_graphics->GetBasePtr()->GetDevice().destroyRenderPass(m_mainRenderPass);
    }

    void Renderer::Record(Commands* commands)
    {  
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto swapchain = m_graphics->GetSwapchainPtr();
        auto& commandBuffers = *commands->GetCommandBuffers();
        
        for (size_t i = 0; i < commandBuffers.size(); ++i)
        {
            swapchain->WaitForFrameFence(true);
            
            auto* cmd = &commandBuffers[i];

            // Begin recording commands to each command buffer.
            cmd->begin(vk::CommandBufferBeginInfo{});
            BeginRenderPass(cmd, swapchain, &m_mainRenderPass, i);
            BindSharedData(cmd);

            if (m_wireframeTechnique)
            {
                m_wireframeTechnique->Bind(cmd);
                m_wireframeTechnique->Record(cmd);
            }

            if (m_particleTechnique)
            {
                m_particleTechnique->Bind(cmd);
                m_particleTechnique->Record(cmd);
            }

            if (m_phongAndUiTechnique)
            {
                m_phongAndUiTechnique->Bind(cmd);
                m_phongAndUiTechnique->Record(cmd);
                RecordUi(cmd);
            }

            // End recording commands to each command buffer.
            cmd->endRenderPass();
            cmd->end();
        }

        NC_PROFILE_END();
    }

    void Renderer::BindSharedData(vk::CommandBuffer* cmd)
    {
        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, ResourceManager::GetVertexBuffer(), offsets);
        cmd->bindIndexBuffer(*ResourceManager::GetIndexBuffer(), 0, vk::IndexType::eUint32);
    }

    void Renderer::BeginRenderPass(vk::CommandBuffer* cmd, vulkan::Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index)
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

    void Renderer::RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates)
    {
        if (!m_particleTechnique)
        {
            m_particleTechnique = std::make_unique<ParticleTechnique>(m_graphics, &m_mainRenderPass);
        }
        m_particleTechnique->RegisterEmitters(m_emitterStates);
    }

    void Renderer::RegisterMeshRenderer(TechniqueType techniqueType, nc::vulkan::MeshRenderer* renderer)
    {
        switch (techniqueType)
        {
            case TechniqueType::PhongAndUi:
            {
                if (!m_phongAndUiTechnique)
                {
                    m_phongAndUiTechnique = std::make_unique<PhongAndUiTechnique>(m_graphics, &m_mainRenderPass);
                }
                m_phongAndUiTechnique->RegisterMeshRenderer(renderer);
                break;
            }
            case TechniqueType::Wireframe:
            {
                if (!m_wireframeTechnique)
                {
                    m_wireframeTechnique = std::make_unique<WireframeTechnique>(m_graphics, &m_mainRenderPass);
                }
                m_wireframeTechnique->RegisterMeshRenderer(renderer);
                break;
            }
            case TechniqueType::None:
            {
                break;
            }
        }
    }

    void Renderer::RecordUi(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
        NC_PROFILE_END();
    }
}