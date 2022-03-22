#include "UiTechnique.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/Swapchain.h"
#include "graphics/VertexDescriptions.h"

namespace nc::graphics
{
    UiTechnique::UiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
        : m_graphics{ graphics },
        m_base{ graphics->GetBasePtr() },
        m_pipeline{ nullptr },
        m_pipelineLayout{ nullptr }
    {
        CreatePipeline(renderPass);
    }

    UiTechnique::~UiTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool UiTechnique::CanBind(const PerFrameRenderState&)
    {
        return true;
    }

    void UiTechnique::Bind(vk::CommandBuffer* cmd)
    {
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    }

    void UiTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetProjectSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "UiVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "UiFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo();
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

    bool UiTechnique::CanRecord(const PerFrameRenderState&)
    {
        return true;
    }

    void UiTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState&)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
    }
}