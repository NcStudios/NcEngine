#include "ShadowMappingTechnique.h"
#include "config/Config.h"
#include "Ecs.h"
#include "component/MeshRenderer.h"
#include "component/PointLight.h"
#include "component/Transform.h"
#include "debug/Profiler.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/MeshManager.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/Swapchain.h"
#include "graphics/resources/ResourceManager.h"


namespace nc::graphics
{
    ShadowMappingTechnique::ShadowMappingTechnique(Graphics* graphics, vk::RenderPass* renderPass)
    : 
      m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_swapchain{graphics->GetSwapchainPtr()},
      m_pipeline{},
      m_pipelineLayout{},
      m_lightProjectionMatrix{}
    {
        const auto& graphicsSettings = config::GetGraphicsSettings();
        m_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(math::DegreesToRadians(LIGHT_FIELD_OF_VIEW), 1.0f, graphicsSettings.nearClip, graphicsSettings.farClip);
        m_modelMatrix = DirectX::XMMatrixSet(
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        );

        CreatePipeline(renderPass);
    }

    ShadowMappingTechnique::~ShadowMappingTechnique()
    {
        auto device = m_base->GetDevice();
        device.destroyPipelineLayout(m_pipelineLayout);
        device.destroyPipeline(m_pipeline);
    }

    void ShadowMappingTechnique::Bind(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, ResourceManager::GetObjectsDescriptorSet(), 0, 0);
        NC_PROFILE_END();
    }

    void ShadowMappingTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "ShadowMappingVertex.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);

        vk::PipelineShaderStageCreateInfo shaderStages[] = 
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(ShadowMappingPushConstants)); // PushConstants
        std::vector<vk::DescriptorSetLayout> descriptorLayouts = {*ResourceManager::GetObjectsDescriptorSetLayout()};
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayout(pipelineLayoutInfo);

        std::array<vk::DynamicState, 3> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eDepthBias };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(1); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages); // Shader stages
        auto vertexBindingDescription = GetVertexBindingDescription();
        auto vertexAttributeDescription = GetVertexAttributeDescriptions();
        auto vertexInputInfo = CreateVertexInputCreateInfo(vertexBindingDescription, vertexAttributeDescription);
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        auto inputAssembly = CreateInputAssemblyCreateInfo();
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        auto viewportState = CreateViewportCreateInfo();
        pipelineCreateInfo.setPViewportState(&viewportState);
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill, 1.0f, true);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMulitsampleCreateInfo();
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlending = CreateColorBlendStateCreateInfo();
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout);
        pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = m_base->GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
    }

    void ShadowMappingTechnique::Record(vk::CommandBuffer* cmd, registry_type* registry, std::span<nc::PointLight> pointLights, std::span<nc::MeshRenderer> meshRenderers)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        cmd->setDepthBias
        (
            DEPTH_BIAS_CONSTANT,
            0.0f,
            DEPTH_BIAS_SLOPE
        );

        auto pushConstants = ShadowMappingPushConstants{};

        for (const auto& pointLight : pointLights)
        {
            auto lightViewMatrix = CalculateViewMatrix(registry->Get<Transform>(pointLight.GetParentEntity()));
            pushConstants.lightViewProjection = lightViewMatrix * m_lightProjectionMatrix;

            cmd->pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ShadowMappingPushConstants), &pushConstants);

            uint32_t objectInstance = 0;
            for (const auto& renderer : meshRenderers)
            {
                auto& mesh = renderer.GetMesh();
                cmd->drawIndexed(mesh.indicesCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                objectInstance++;
            }
        }

        NC_PROFILE_END();
    }

    DirectX::XMMATRIX ShadowMappingTechnique::CalculateViewMatrix(Transform* lightTransform)
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, lightTransform->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtRH(pos_v, pos_v + look_v, DirectX::g_XMNegIdentityR1);
    }
}