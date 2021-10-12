#include "Renderer.h"
#include "Ecs.h"
#include "component/DebugWidget.h"
#include "component/MeshRenderer.h"
#include "component/Transform.h"
#include "debug/Profiler.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "graphics/resources/ResourceManager.h"
#include "graphics/techniques/PhongAndUiTechnique.h"
#include "graphics/Swapchain.h"
#include "PerFrameRenderState.h"

#include <span>

namespace nc::graphics
{
    Renderer::Renderer(graphics::Graphics* graphics)
        : m_graphics{graphics},
          m_textureManager{graphics},
          m_meshManager{graphics},
          m_mainRenderPass{m_graphics->GetSwapchainPtr()->GetPassDefinition()},
          m_shadowMappingPass{},
          m_phongAndUiTechnique{nullptr}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_wireframeTechnique{nullptr}
          #endif
          //m_particleTechnique{nullptr}
    {
        graphics::ResourceManager::InitializeShadowMap(graphics);
        InitializeShadowMappingRenderPass();
    }

    Renderer::~Renderer() noexcept
    {
        m_graphics->GetBasePtr()->GetDevice().destroyRenderPass(m_mainRenderPass);
    }

    void Renderer::InitializeTechniques()
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

        if (m_shadowMappingTechnique == nullptr)
        {
            m_shadowMappingTechnique = std::make_unique<ShadowMappingTechnique>(m_graphics, &m_shadowMappingPass.renderPass.get());
        }
    }

    void Renderer::Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        InitializeTechniques();
        
        auto swapchain = m_graphics->GetSwapchainPtr();
        swapchain->WaitForFrameFence();
        auto& commandBuffers = *commands->GetCommandBuffers();
        auto* cmd = &commandBuffers[currentSwapChainImageIndex];
        cmd->begin(vk::CommandBufferBeginInfo{});

        // Shadow mapping pass
        {
            BeginRenderPass(cmd, &m_shadowMappingPass.renderPass.get(), m_shadowMappingPass.frameBuffer.get(), swapchain->GetExtent(), ClearValue::Depth);

            m_shadowMappingTechnique->Bind(cmd);
            m_shadowMappingTechnique->Record(cmd, state.pointLightVPs, state.meshes);

            cmd->endRenderPass();
        }

        // Lit shading pass
        {
            BeginRenderPass(cmd, &m_mainRenderPass, swapchain->GetFrameBuffer(currentSwapChainImageIndex), swapchain->GetExtent(), ClearValue::DepthAndColor);

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

            // End recording commands to each command buffer.
            cmd->endRenderPass();
        }

        cmd->end();
        NC_PROFILE_END();
    }

    void Renderer::BindSharedData(vk::CommandBuffer* cmd)
    {
        vk::DeviceSize offsets[] = { 0 };
        cmd->bindVertexBuffers(0, 1, ResourceManager::GetVertexData().buffer.GetBuffer(), offsets);
        cmd->bindIndexBuffer(*ResourceManager::GetIndexData().buffer.GetBuffer(), 0, vk::IndexType::eUint32);
    }

    void Renderer::BeginRenderPass(vk::CommandBuffer* cmd, vk::RenderPass* renderPass, vk::Framebuffer& framebuffer, const vk::Extent2D& extent, ClearValue clearValue)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto clearValues = CreateClearValues(clearValue, m_graphics->GetClearColor());
        auto renderPassInfo = CreateRenderPassBeginInfo(*renderPass, 
                                                        framebuffer, 
                                                        extent, 
                                                        clearValues);

        cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        auto dimensions = m_graphics->GetDimensions();
        SetViewportAndScissor(cmd, dimensions);

        BindSharedData(cmd);

        NC_PROFILE_END();
    }

    void Renderer::InitializeShadowMappingRenderPass()
    {
        auto device = m_graphics->GetBasePtr()->GetDevice();

        std::array<vk::AttachmentDescription, 1> renderPassAttachments = 
        {
            CreateAttachmentDescription(AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore)
        };

        vk::AttachmentReference depthReference = CreateAttachmentReference(AttachmentType::Depth, 0);
        vk::SubpassDescription subpass = CreateSubpassDescription(depthReference);

        std::array<vk::SubpassDependency, 2> subpassDependencies =
        {
            CreateSubpassDependency(VK_SUBPASS_EXTERNAL,
                                    0,
                                    vk::PipelineStageFlagBits::eFragmentShader,
                                    vk::PipelineStageFlagBits::eEarlyFragmentTests,
                                    vk::AccessFlagBits::eShaderRead,
                                    vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                                    vk::DependencyFlagBits::eByRegion),

            CreateSubpassDependency(0,
                                    VK_SUBPASS_EXTERNAL,
                                    vk::PipelineStageFlagBits::eLateFragmentTests,
                                    vk::PipelineStageFlagBits::eFragmentShader,
                                    vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                                    vk::AccessFlagBits::eShaderRead,
                                    vk::DependencyFlagBits::eByRegion),
        };

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(1);
        renderPassInfo.setPAttachments(renderPassAttachments.data());
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpass);
        renderPassInfo.setDependencyCount(static_cast<uint32_t>(subpassDependencies.size()));
        renderPassInfo.setPDependencies(subpassDependencies.data());

        m_shadowMappingPass.renderPass = device.createRenderPassUnique(renderPassInfo);

        // Create frame buffer
        std::array<vk::ImageView, 1> attachments;
        attachments[0] = ResourceManager::GetShadowMapImageView(); // Depth Stencil image view
        auto dimensions = m_graphics->GetDimensions();

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(m_shadowMappingPass.renderPass.get());
        framebufferInfo.setAttachmentCount(1);
        framebufferInfo.setPAttachments(attachments.data());
        framebufferInfo.setWidth(dimensions.x);
        framebufferInfo.setHeight(dimensions.y);
        framebufferInfo.setLayers(1);

        m_shadowMappingPass.frameBuffer = device.createFramebufferUnique(framebufferInfo);
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