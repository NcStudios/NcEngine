#include "PhongAndUiTechnique.h"
#include "Ecs.h"
#include "config/Config.h"
#include "component/Transform.h"
#include "component/MeshRenderer.h"
#include "debug/Profiler.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "graphics/Initializers.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/MeshManager.h"
#include "graphics/Swapchain.h"
#include "graphics/Base.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/resources/ResourceManager.h"

namespace nc::graphics
{
    PhongAndUiTechnique::PhongAndUiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
    : m_meshRenderers{},
      m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_swapchain{graphics->GetSwapchainPtr()},
      m_pipeline{},
      m_pipelineLayout{}
    {
        CreatePipeline(renderPass);
    }

    PhongAndUiTechnique::~PhongAndUiTechnique()
    {
        auto device = m_base->GetDevice();
        device.destroyPipelineLayout(m_pipelineLayout);
        device.destroyPipeline(m_pipeline);
    }

    void PhongAndUiTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "PhongVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "PhongFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        vk::PipelineShaderStageCreateInfo shaderStages[] = 
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment, sizeof(PhongPushConstants)); // PushConstants
        std::vector<vk::DescriptorSetLayout> descriptorLayouts = {*ResourceManager::GetTexturesDescriptorSetLayout(), *ResourceManager::GetPointLightsDescriptorSetLayout(), *ResourceManager::GetObjectsDescriptorSetLayout(), *ResourceManager::GetShadowMapDescriptorSetLayout()};
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
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
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill, 1.0f);
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
        pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = m_base->GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    void PhongAndUiTechnique::Bind(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, ResourceManager::GetTexturesDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 1, 1, ResourceManager::GetPointLightsDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 2, 1, ResourceManager::GetObjectsDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 3, 1, ResourceManager::GetShadowMapDescriptorSet(), 0, 0);
        NC_PROFILE_END();
    }

    void PhongAndUiTechnique::Record(vk::CommandBuffer* cmd, std::span<nc::MeshRenderer> meshRenderers)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        auto pushConstants = PhongPushConstants{};
        pushConstants.cameraPos = m_graphics->GetCameraPosition();
        cmd->pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, sizeof(PhongPushConstants), &pushConstants);

        uint32_t objectInstance = 0;
        for (auto& renderer : meshRenderers)
        {
            auto& mesh = renderer.GetMesh();
            cmd->drawIndexed(mesh.indicesCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            
            #ifdef NC_EDITOR_ENABLED
            m_graphics->IncrementDrawCallCount();
            #endif
            
            objectInstance++;
        }
        NC_PROFILE_END();
    }

    void PhongAndUiTechnique::Clear()
    {
        m_meshRenderers.clear();
    }
}