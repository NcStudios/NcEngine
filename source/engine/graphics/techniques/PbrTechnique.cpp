#include "PbrTechnique.h"
#include "asset/Assets.h"
#include "config/Config.h"
#include "graphics/buffers/ImmutableBuffer.h"
#include "graphics/GpuOptions.h"
#include "graphics/Initializers.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/meshes/VertexDescriptions.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResourceServices.h"
#include "graphics/shaders/ShaderUtilities.h"
#include "optick/optick.h"

namespace nc::graphics
{
    PbrTechnique::PbrTechnique(vk::Device device, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass)
        : m_descriptorSets{descriptorSets},
          m_pipeline{nullptr},
          m_pipelineLayout{nullptr}
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "PbrVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "PbrFragment.spv");

        auto vertexShaderModule = CreateShaderModule(device, vertexShaderByteCode);
        auto fragmentShaderModule = CreateShaderModule(device, fragmentShaderByteCode);

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
        m_pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

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
        auto multisampling = CreateMultisampleCreateInfo(gpuOptions->GetMaxSamplesCount());
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

        m_pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;

        device.destroyShaderModule(vertexShaderModule, nullptr);
        device.destroyShaderModule(fragmentShaderModule, nullptr);
    }

    PbrTechnique::~PbrTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool PbrTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        (void)frameData;
        return true;
    }

    void PbrTechnique::Bind(vk::CommandBuffer* cmd)
    {
        OPTICK_CATEGORY("PbrTechnique::Bind", Optick::Category::Rendering);

        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        m_descriptorSets->BindSet(BindFrequency::per_frame, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
    }

    bool PbrTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        (void)frameData;
        return true;
    }

    void PbrTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("PbrTechnique::Record", Optick::Category::Rendering);
        uint32_t objectInstance = 0;
        for (const auto& mesh : frameData.meshes)
        {
            cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            ++objectInstance;
        }
    }

    void PbrTechnique::Clear() noexcept
    {
    }
}