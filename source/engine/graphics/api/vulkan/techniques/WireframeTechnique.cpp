#ifdef NC_EDITOR_ENABLED
#include "WireframeTechnique.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/meshes/VertexDescriptions.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "graphics/PerFrameRenderState.h"

namespace nc::graphics
{
    WireframeTechnique::WireframeTechnique(const Device& device, ShaderDescriptorSets*, vk::RenderPass* renderPass)
        : m_pipeline{nullptr},
          m_pipelineLayout{nullptr}
    {
        const auto vkDevice = device.VkDevice();

        // Shaders
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "WireframeVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "WireframeFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vkDevice, vertexShaderByteCode);
        auto fragmentShaderModule = CreateShaderModule(vkDevice, fragmentShaderByteCode);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRanges = std::array{
            CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(WireframeVertexPushConstants), 0u),
            CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment, sizeof(WireframeFragmentPushConstants), sizeof(WireframeVertexPushConstants))
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRanges);
        m_pipelineLayout = vkDevice.createPipelineLayoutUnique(pipelineLayoutInfo);

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
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eLine);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMultisampleCreateInfo(device.GetGpuOptions().GetMaxSamplesCount());
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

        m_pipeline = vkDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
        vkDevice.destroyShaderModule(vertexShaderModule, nullptr);
        vkDevice.destroyShaderModule(fragmentShaderModule, nullptr);
    }

    WireframeTechnique::~WireframeTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool WireframeTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        return !frameData.widgetState.wireframeData.empty();
    }

    void WireframeTechnique::Bind(vk::CommandBuffer* cmd)
    {
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    }

    bool WireframeTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        return !frameData.widgetState.wireframeData.empty();
    }

    void WireframeTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        auto vertexPushConstants = WireframeVertexPushConstants{
            .model = {},
            .viewProjection = frameData.cameraState.view * frameData.cameraState.projection
        };

        auto fragmentPushConstants = WireframeFragmentPushConstants{
            .color = Vector4{1.0f, 0.0f, 0.0f, 1.0f}
        };

        for (const auto& [matrix, mesh, color] : frameData.widgetState.wireframeData)
        {
            vertexPushConstants.model = matrix;
            fragmentPushConstants.color = color;
            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframeVertexPushConstants), &vertexPushConstants);
            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eFragment, sizeof(WireframeVertexPushConstants), sizeof(WireframeFragmentPushConstants), &fragmentPushConstants.color);
            cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }
    }
}
#endif
