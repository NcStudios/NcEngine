#ifdef NC_EDITOR_ENABLED
#include "WireframePipeline.h"
#include "asset/AssetService.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/VertexDescriptions.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderUtilities.h"
#include "graphics/PerFrameRenderState.h"

#include "optick.h"

namespace nc::graphics::vulkan
{
WireframePipeline::WireframePipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass)
    : m_shaderBindingManager{shaderBindingManager},
      m_pipeline{nullptr},
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
        CreatePipelineShaderStageCreateInfo(ShaderStage::Fragment, fragmentShaderModule)
    };

    m_pipelineLayout = [vkDevice, shaderBindingManager]()
    {
        const auto descriptorLayout = *(shaderBindingManager->GetSetLayout(0));
        const auto pushConstantRanges = std::array{
            CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(WireframeVertexPushConstants), 0u),
            CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment, sizeof(WireframeFragmentPushConstants), sizeof(WireframeVertexPushConstants))
        };

        const auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{
            vk::PipelineLayoutCreateFlags{},
            1u,
            &descriptorLayout,
            static_cast<uint32_t>(pushConstantRanges.size()),
            pushConstantRanges.data()
        };

        return vkDevice.createPipelineLayoutUnique(pipelineLayoutInfo);
    }();

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
    pipelineCreateInfo.setRenderPass(renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/specs/1.0/html/vkspec.html#renderpass-compatibility
    pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
    pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
    pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

    m_pipeline = vkDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
    vkDevice.destroyShaderModule(vertexShaderModule, nullptr);
    vkDevice.destroyShaderModule(fragmentShaderModule, nullptr);
}

WireframePipeline::~WireframePipeline() noexcept
{
    m_pipeline.reset();
    m_pipelineLayout.reset();
}

void WireframePipeline::Bind(uint32_t frameIndex, vk::CommandBuffer* cmd)
{
    cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    m_shaderBindingManager->BindSet(0, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0, frameIndex);
}

void WireframePipeline::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&)
{
    OPTICK_CATEGORY("WireframePipeline::Record", Optick::Category::Rendering);

    auto vertexPushConstants = WireframeVertexPushConstants{};
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
} // namespace nc::graphics::vulkan

#endif
