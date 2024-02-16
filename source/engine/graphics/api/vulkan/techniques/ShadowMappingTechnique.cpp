#include "ShadowMappingTechnique.h"
#include "config/Config.h"
#include "graphics/api/vulkan/core/GpuOptions.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/meshes/VertexDescriptions.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"
#include "graphics/api/vulkan/shaders/ShaderResources.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "graphics/MeshRenderer.h"
#include "graphics/PerFrameRenderState.h"

#include "optick.h"

namespace
{
    // Depth bias (and slope) are used to avoid shadowing artifacts
    // Constant depth bias factor (always applied)
    constexpr float DEPTH_BIAS_CONSTANT = 1.25f;

    // Slope depth bias factor, applied depending on polygon's slope
    constexpr float DEPTH_BIAS_SLOPE = 1.75f;
}

namespace nc::graphics
{
    ShadowMappingTechnique::ShadowMappingTechnique(vk::Device device, ShaderDescriptorSets* descriptorSets, vk::RenderPass renderPass, uint32_t shadowCasterIndex)
        : m_descriptorSets{descriptorSets},
          m_pipeline{nullptr},
          m_pipelineLayout{nullptr},
          m_enabled{false},
          m_shadowCasterIndex{shadowCasterIndex}
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "ShadowMappingVertex.spv");
        auto vertexShaderModule = CreateShaderModule(device, vertexShaderByteCode);

        std::array<vk::PipelineShaderStageCreateInfo, 1u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule)
        };

        // Shader data
        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(ShadowMappingPushConstants)); // Push Constants

        std::array<vk::DescriptorSetLayout, 1u> descriptorLayouts
        {
            *(m_descriptorSets->GetSetLayout(BindFrequency::per_frame))
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
        m_pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

        // Graphics pipeline
        std::array<vk::DynamicState, 3> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eDepthBias };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicStateInfo.setDynamicStates(dynamicStates);

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(1); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages.data()); // Shader stages
        auto vertexBindingDescription = GetVertexBindingDescription();
        auto vertexAttributeDescription = GetVertexAttributeDescriptions();
        auto vertexInputInfo = CreateVertexInputCreateInfo(vertexBindingDescription, vertexAttributeDescription);
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        auto inputAssembly = CreateInputAssemblyCreateInfo();
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        auto viewportState = CreateViewportCreateInfo();
        pipelineCreateInfo.setPViewportState(&viewportState);
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eFront, 1.0f, true);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMultisampleCreateInfo(vk::SampleCountFlagBits::e1);
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlending = CreateColorBlendStateCreateInfo();
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout.get());
        pipelineCreateInfo.setRenderPass(renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
        device.destroyShaderModule(vertexShaderModule, nullptr);
    }

    ShadowMappingTechnique::~ShadowMappingTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool ShadowMappingTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        static const auto useShadows = config::GetGraphicsSettings().useShadows;
        return m_enabled = useShadows && !frameData.lightingState.viewProjections.empty();
    }

    void ShadowMappingTechnique::Bind(vk::CommandBuffer* cmd)
    {
        OPTICK_CATEGORY("ShadowMappingTechnique::Bind", Optick::Category::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        m_descriptorSets->BindSet(BindFrequency::per_frame, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
    }

    bool ShadowMappingTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        (void)frameData;
        return m_enabled;
    }

    void ShadowMappingTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("ShadowMappingTechnique::Record", Optick::Category::Rendering);
        cmd->setDepthBias
        (
            DEPTH_BIAS_CONSTANT,
            0.0f,
            DEPTH_BIAS_SLOPE
        );

        auto pushConstants = ShadowMappingPushConstants{};

        // We are rendering the position of each mesh renderer's vertex in respect to each point light's view space.
        if (m_shadowCasterIndex >= frameData.lightingState.viewProjections.size())
        {
            throw nc::NcError("Shadow caster index is out of bounds.");
        }
        pushConstants.lightViewProjection = frameData.lightingState.viewProjections[m_shadowCasterIndex];

        cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(ShadowMappingPushConstants), &pushConstants);

        uint32_t objectInstance = 0;
        for (const auto& mesh : frameData.objectState.pbrMeshes)
        {
            cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            objectInstance++;
        }
        for (const auto& mesh : frameData.objectState.toonMeshes)
        {
            cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            objectInstance++;
        }
    }
} // namespace nc::graphics
