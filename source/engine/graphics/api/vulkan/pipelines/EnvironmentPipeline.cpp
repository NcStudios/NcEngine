#include "EnvironmentPipeline.h"
#include "asset/Assets.h"
#include "asset/AssetService.h"
#include "config/Config.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/ShaderUtilities.h"
#include "graphics/api/vulkan/VertexDescriptions.h"
#include "graphics/PerFrameRenderState.h"

#include "optick.h"

namespace nc::graphics::vulkan
{
EnvironmentPipeline::EnvironmentPipeline(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass renderPass)
    : m_shaderBindingManager{shaderBindingManager},
      m_pipeline{nullptr},
      m_pipelineLayout{nullptr}
{
    const auto vkDevice = device.VkDevice();

    // Shaders
    auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
    auto vertexShaderByteCode = ReadShader(defaultShaderPath + "EnvironmentVertex.spv");
    auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "EnvironmentFragment.spv");

    auto vertexShaderModule = CreateShaderModule(vkDevice, vertexShaderByteCode);
    auto fragmentShaderModule = CreateShaderModule(vkDevice, fragmentShaderByteCode);

    std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
    {
        CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
        CreatePipelineShaderStageCreateInfo(ShaderStage::Fragment, fragmentShaderModule)
    };

    std::array<vk::DescriptorSetLayout, 2u> descriptorLayouts
    {
        *(m_shaderBindingManager->GetSetLayout(0)),
        *(m_shaderBindingManager->GetSetLayout(1))
    };

    auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(descriptorLayouts);
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
    auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill);
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

EnvironmentPipeline::~EnvironmentPipeline() noexcept
{
    m_pipeline.reset();
    m_pipelineLayout.reset();
}

void EnvironmentPipeline::Bind(uint32_t frameIndex, vk::CommandBuffer* cmd)
{
    OPTICK_CATEGORY("EnvironmentPipeline::Bind", Optick::Category::Rendering);
    cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    m_shaderBindingManager->BindSet(0, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0, frameIndex);
    m_shaderBindingManager->BindSet(1, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
}

void EnvironmentPipeline::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData&)
{
    OPTICK_CATEGORY("EnvironmentPipeline::Record", Optick::Category::Rendering);
    if (!frameData.environmentState.useSkybox)
    {
        return;
    }

    const auto meshAccessor = asset::AssetService<asset::MeshView>::Get()->Acquire(nc::asset::SkyboxMesh);
    cmd->drawIndexed
    (
        meshAccessor.indexCount,                          // indexCount
        1u,                                               // instanceCount
        meshAccessor.firstIndex,                          // firstIndex
        meshAccessor.firstVertex,                         // vertexOffset
        frameData.objectState.skyboxInstanceIndex.value() // firstInstance
    );
}

void EnvironmentPipeline::Clear() noexcept
{
}
} // namespace nc::graphics::vulkan
