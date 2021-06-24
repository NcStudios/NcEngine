#include "WireframeTechnique.h"
#include "Ecs.h"
#include "config/Config.h"
#include "component/Transform.h"
#include "component/vulkan/MeshRenderer.h"
#include "debug/Profiler.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Initializers.h"
#include "graphics/vulkan/resources/ImmutableBuffer.h"
#include "graphics/vulkan/ShaderUtilities.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/resources/ResourceManager.h"

namespace nc::graphics::vulkan
{
    WireframeTechnique::WireframeTechnique(nc::graphics::Graphics2* graphics, vk::RenderPass* renderPass)
    : m_meshRenderers{},
      m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_swapchain{graphics->GetSwapchainPtr()},
      m_pipeline{},
      m_pipelineLayout{},
      m_descriptorSetLayout{}
    {
        CreatePipeline(renderPass);
    }

    WireframeTechnique::~WireframeTechnique()
    {
        auto device = m_base->GetDevice();
        device.destroyDescriptorSetLayout(m_descriptorSetLayout);
        device.destroyPipelineLayout(m_pipelineLayout);
        device.destroyPipeline(m_pipeline);
    }

    void WireframeTechnique::Bind(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        NC_PROFILE_END();
    }

    void WireframeTechnique::RegisterMeshRenderer(nc::vulkan::MeshRenderer* meshRenderer)
    {
        auto renderers = m_meshRenderers.find(meshRenderer->GetMeshUid());
        if (renderers == m_meshRenderers.end())
        {

            m_meshRenderers.emplace(meshRenderer->GetMeshUid(), std::vector<Entity>{meshRenderer->GetParentEntity()} );
            return;
        }
        
        renderers->second.push_back(meshRenderer->GetParentEntity());
    }

    void WireframeTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().vulkanShadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "WireframeVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "WireframeFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        vk::PipelineShaderStageCreateInfo shaderStages[] = 
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, sizeof(WireframePushConstants)); // PushConstants
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayout(pipelineLayoutInfo);

        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(2); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages); // Shader stages
        auto vertexBindingDescription = GetVertexBindingDescription();
        auto vertexAttributeDescription = GetVertexAttributeDescriptions();
        auto vertexInputInfo = CreateVertexInputCreateInfo(vertexBindingDescription, vertexAttributeDescription);
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        auto inputAssembly = CreateInputAssemblyCreateInfo();
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        auto viewportState = CreateViewportCreateInfo();
        pipelineCreateInfo.setPViewportState(&viewportState);
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eLine, 2.0f);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMulitsampleCreateInfo();
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlendAttachment = CreateColorBlendAttachmentCreateInfo(false);
        auto colorBlending = CreateColorBlendStateCreateInfo(colorBlendAttachment, false);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout);
        pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = m_base->GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    void WireframeTechnique::Record(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        const auto& viewMatrix = m_graphics->GetViewMatrix();
        const auto& projectionMatrix = m_graphics->GetProjectionMatrix();

        auto pushConstants = WireframePushConstants{};
        pushConstants.viewProjection = viewMatrix * projectionMatrix;
        
        for (auto& [meshUid, renderers] : m_meshRenderers)
        {
            const auto meshAccessor = ResourceManager::GetMeshAccessor(meshUid);
            
            for (auto handle : renderers)
            {
                auto* meshRenderer = ActiveRegistry()->Get<nc::vulkan::MeshRenderer>(handle);
                pushConstants.model = meshRenderer->GetTransform()->GetTransformationMatrix();
                pushConstants.normal = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, pushConstants.model));

                cmd->pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
                cmd->drawIndexed(meshAccessor.indicesCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                
                #ifdef NC_EDITOR_ENABLED
                m_graphics->IncrementDrawCallCount();
                #endif
            }
        }
        NC_PROFILE_END();
    }

}