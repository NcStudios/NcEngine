#include "EnvironmentTechnique.h"
#include "assets/AssetUtilities.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "graphics/Initializers.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/Swapchain.h"
#include "graphics/Base.h"
#include "graphics/VertexDescriptions.h"
#include "graphics/resources/Environment.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/resources/ShaderResourceServices.h"
#include "optick/optick.h"

namespace nc::graphics
{
    struct Texture;

    EnvironmentTechnique::EnvironmentTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
    : m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_descriptorSets{ m_graphics->GetShaderResources()->GetDescriptorSets() },
      m_pipeline{nullptr},
      m_pipelineLayout{nullptr}
    {
        CreatePipeline(renderPass);
    }

    EnvironmentTechnique::~EnvironmentTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    void EnvironmentTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetProjectSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "EnvironmentVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "EnvironmentFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        std::array<vk::DescriptorSetLayout, 1u> descriptorLayouts
        {
            *(m_descriptorSets->GetSetLayout(BindFrequency::per_frame))
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(descriptorLayouts);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(static_cast<uint32_t>(shaderStages.size())); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages.data()); // Shader stages
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
        auto multisampling = CreateMulitsampleCreateInfo(m_base->GetMaxSamplesCount());
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlendAttachment = CreateColorBlendAttachmentCreateInfo(false);
        auto colorBlending = CreateColorBlendStateCreateInfo(colorBlendAttachment, false);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout.get());
        pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.
        
        m_pipeline = m_base->GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
       
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    bool EnvironmentTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        return frameData.useSkybox;
    }

    void EnvironmentTechnique::Bind(vk::CommandBuffer* cmd)
    {
        OPTICK_CATEGORY("EnvironmentTechnique::Bind", Optick::Category::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        m_descriptorSets->BindSet(BindFrequency::per_frame, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
    }

    bool EnvironmentTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        return frameData.useSkybox;
    }

    void EnvironmentTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("EnvironmentTechnique::Record", Optick::Category::Rendering);
        const auto objectCount = static_cast<uint32_t>(frameData.objectData.size());
        if (objectCount == 0) return;
        const auto meshAccessor = AssetService<MeshView>::Get()->Acquire(nc::SkyboxMeshPath);
        cmd->drawIndexed(meshAccessor.indexCount, 1u, meshAccessor.firstIndex, meshAccessor.firstVertex, objectCount - 1); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
    }

    void EnvironmentTechnique::Clear() noexcept
    {
    }
}