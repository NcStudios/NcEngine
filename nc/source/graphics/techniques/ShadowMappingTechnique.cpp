#include "ShadowMappingTechnique.h"
#include "config/Config.h"
#include "Ecs.h"
#include "debug/Profiler.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/resources/ShaderResourceService.h"
#include "graphics/VertexDescriptions.h"

namespace
{
    // Depth bias (and slope) are used to avoid shadowing artifacts
    // Constant depth bias factor (always applied)
    constexpr float DEPTH_BIAS_CONSTANT = 4.0f;

    // Slope depth bias factor, applied depending on polygon's slope
    constexpr float DEPTH_BIAS_SLOPE = 1.5f;
}

namespace nc::graphics
{
    ShadowMappingTechnique::ShadowMappingTechnique(Graphics* graphics, vk::RenderPass* renderPass)
    : 
      m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_swapchain{graphics->GetSwapchainPtr()},
      m_pipeline{},
      m_pipelineLayout{}
    {
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
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, ShaderResourceService<ObjectData>::Get()->GetDescriptorSet(), 0, 0);
        NC_PROFILE_END();
    }

    void ShadowMappingTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "ShadowMappingVertex.spv");
        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);

        std::array<vk::PipelineShaderStageCreateInfo, 1u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule)
        };

        // Shader data
        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(ShadowMappingPushConstants)); // Push Constants

        std::array<vk::DescriptorSetLayout, 1u> descriptorLayouts
        {
            *ShaderResourceService<ObjectData>::Get()->GetDescriptorSetLayout()
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayout(pipelineLayoutInfo);

        // Graphics pipeline
        std::array<vk::DynamicState, 3> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eDepthBias };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(1); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages.data()); // Shader stages
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

    void ShadowMappingTechnique::Record(vk::CommandBuffer* cmd, std::span<const DirectX::XMMATRIX> pointLightVPs, std::span<const MeshView> meshes)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        cmd->setDepthBias
        (
            DEPTH_BIAS_CONSTANT,
            0.0f,
            DEPTH_BIAS_SLOPE
        );

        auto pushConstants = ShadowMappingPushConstants{};

        // We are rendering the position of each mesh renderer's vertex in respect to each point light's view space.
        for (const auto& pointLightVP : pointLightVPs)
        {
            pushConstants.lightViewProjection = pointLightVP;

            cmd->pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ShadowMappingPushConstants), &pushConstants);

            uint32_t objectInstance = 0;
            for (const auto& mesh : meshes)
            {
                cmd->drawIndexed(mesh.indicesCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                objectInstance++;
            }
        }

        NC_PROFILE_END();
    }
}