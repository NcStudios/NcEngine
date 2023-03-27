#include "OutlineTechnique.h"
#include "asset/Assets.h"
#include "config/Config.h"
#include "graphics/api/vulkan/buffers/ImmutableBuffer.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/meshes/VertexDescriptions.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"
#include "graphics/api/vulkan/shaders/ShaderResources.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "graphics/PerFrameRenderState.h"

#include "optick/optick.h"

namespace nc::graphics
{
OutlineTechnique::OutlineTechnique(const Device& device, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass)
    : m_descriptorSets{descriptorSets},
        m_pipeline{nullptr},
        m_pipelineLayout{nullptr}
{
    const auto vkDevice = device.VkDevice();

    // Shaders
    auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
    auto vertexShaderByteCode = ReadShader(defaultShaderPath + "OutlineVertex.spv");
    auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "OutlineFragment.spv");

    auto vertexShaderModule = CreateShaderModule(vkDevice, vertexShaderByteCode);
    auto fragmentShaderModule = CreateShaderModule(vkDevice, fragmentShaderByteCode);

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
    m_pipelineLayout = vkDevice.createPipelineLayoutUnique(pipelineLayoutInfo);

    std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
    dynamicStateInfo.setDynamicStates(dynamicStates);

    auto depthStencil = CreateDepthStencilCreateInfo(true);
    depthStencil.stencilTestEnable = VK_TRUE;
    depthStencil.back.compareOp = vk::CompareOp::eNotEqual;
    depthStencil.back.failOp = vk::StencilOp::eKeep;
    depthStencil.back.depthFailOp = vk::StencilOp::eKeep;
    depthStencil.back.passOp = vk::StencilOp::eReplace;
    depthStencil.back.compareMask = 0xff;
    depthStencil.back.writeMask = 0xff;
    depthStencil.back.reference = 1;
    depthStencil.front = depthStencil.back;
    depthStencil.depthTestEnable = VK_FALSE;

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
    rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    pipelineCreateInfo.setPRasterizationState(&rasterizer);
    auto multisampling = CreateMultisampleCreateInfo(device.GetGpuOptions().GetMaxSamplesCount());
    pipelineCreateInfo.setPMultisampleState(&multisampling);
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

OutlineTechnique::~OutlineTechnique() noexcept
{
    m_pipeline.reset();
    m_pipelineLayout.reset();
}

bool OutlineTechnique::CanBind(const PerFrameRenderState& frameData)
{
    (void)frameData;
    return true;
}

void OutlineTechnique::Bind(vk::CommandBuffer* cmd)
{
    OPTICK_CATEGORY("OutlineTechnique::Bind", Optick::Category::Rendering);

    cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    m_descriptorSets->BindSet(BindFrequency::per_frame, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
}

bool OutlineTechnique::CanRecord(const PerFrameRenderState& frameData)
{
    (void)frameData;
    return true;
}

void OutlineTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
{
    OPTICK_CATEGORY("OutlineTechnique::Record", Optick::Category::Rendering);
    uint32_t objectInstance = 0;
    for (const auto& mesh : frameData.objectState.toonMeshes)
    {
        cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance + frameData.objectState.toonMeshStartingIndex); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        ++objectInstance;
    }
}

void OutlineTechnique::Clear() noexcept
{
}
}