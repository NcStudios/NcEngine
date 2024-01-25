#include "Initializers.h"

namespace nc::graphics
{
auto CreateTextureSampler(vk::Device device, vk::SamplerAddressMode addressMode, uint32_t mipLevels) -> vk::UniqueSampler
{
    const auto samplerInfo = vk::SamplerCreateInfo
    {
        vk::SamplerCreateFlags{},         // flags
        vk::Filter::eLinear,              // magFilter
        vk::Filter::eLinear,              // minFilter
        vk::SamplerMipmapMode::eLinear,   // mipmapMode
        addressMode,                      // addressModeU
        addressMode,                      // addressModeV
        addressMode,                      // addressModeW
        0.0f,                             // mipLodBias
        VK_FALSE,                         // anisotropyEnable
        1.0f,                             // maxAnisotropy
        VK_FALSE,                         // compareEnable
        vk::CompareOp::eAlways,           // compareOp
        0.0f,                             // minLod
        static_cast<float>(mipLevels),    // maxLod
        vk::BorderColor::eIntOpaqueWhite, // borderColor
        VK_FALSE                          // unnormalizedCoordinates
    };

    return device.createSamplerUnique(samplerInfo);
}

auto CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader) -> vk::PipelineShaderStageCreateInfo
{
    const auto shaderStageFlags = stage == ShaderStage::Vertex ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment;
    return vk::PipelineShaderStageCreateInfo
    {
        vk::PipelineShaderStageCreateFlags{},
        shaderStageFlags,
        shader,
        "main",
        nullptr
    };
}

vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, std::span<const vk::VertexInputAttributeDescription> vertexAttributeDesc)
{
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptionCount(1);
    vertexInputInfo.setPVertexBindingDescriptions(&vertexInputDesc);
    vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributeDesc.size()));
    vertexInputInfo.setPVertexAttributeDescriptions(vertexAttributeDesc.data());
    return vertexInputInfo;
}

vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo()
{
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    return vertexInputInfo;
}

vk::PipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo()
{
    // @todo: Look into using element buffers here to reuse vertices
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssembly.setPrimitiveRestartEnable(static_cast<vk::Bool32>(false));
    return inputAssembly;
}

vk::PipelineViewportStateCreateInfo CreateViewportCreateInfo()
{
    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewportCount(1);
    viewportState.setPViewports(nullptr);
    viewportState.setScissorCount(1);
    viewportState.setPScissors(nullptr);
    return viewportState;
}

vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, float lineWidth, bool depthBiasEnable)
{
    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable(static_cast<vk::Bool32>(false)); // Set to false for shadow mapping, requires enabling a GPU feature.
    rasterizer.setRasterizerDiscardEnable(static_cast<vk::Bool32>(false));
    rasterizer.setPolygonMode(polygonMode);
    rasterizer.setLineWidth(lineWidth);
    rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);
    rasterizer.setDepthBiasEnable(static_cast<vk::Bool32>(depthBiasEnable));
    rasterizer.setDepthBiasConstantFactor(0.0f);
    rasterizer.setDepthBiasClamp(0.0f);
    rasterizer.setDepthBiasSlopeFactor(0.0f);
    return rasterizer;
}

vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, float lineWidth, bool depthBiasEnable)
{
    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable(static_cast<vk::Bool32>(false)); // Set to false for shadow mapping, requires enabling a GPU feature.
    rasterizer.setRasterizerDiscardEnable(static_cast<vk::Bool32>(false));
    rasterizer.setPolygonMode(polygonMode);
    rasterizer.setLineWidth(lineWidth);
    rasterizer.setCullMode(cullMode);
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);
    rasterizer.setDepthBiasEnable(static_cast<vk::Bool32>(depthBiasEnable));
    rasterizer.setDepthBiasConstantFactor(0.0f);
    rasterizer.setDepthBiasClamp(0.0f);
    rasterizer.setDepthBiasSlopeFactor(0.0f);
    return rasterizer;
}

vk::PipelineMultisampleStateCreateInfo CreateMultisampleCreateInfo(vk::SampleCountFlagBits numSamples)
{
    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setSampleShadingEnable(static_cast<vk::Bool32>(false));
    multisampling.setRasterizationSamples(numSamples);
    multisampling.setMinSampleShading(0.0f);
    multisampling.setPSampleMask(nullptr);
    multisampling.setAlphaToCoverageEnable(static_cast<vk::Bool32>(false));
    multisampling.setAlphaToOneEnable(static_cast<vk::Bool32>(false));
    return multisampling;
}

vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo(bool shadowMapping)
{
    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.setDepthTestEnable(static_cast<vk::Bool32>(true));
    depthStencil.setDepthWriteEnable(static_cast<vk::Bool32>(true));

    if (shadowMapping)
    {
        depthStencil.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
    }
    else
    {
        depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
    }
    return depthStencil;
}

vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo(bool useAlphaBlending)
{
    if (useAlphaBlending)
    {
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        colorBlendAttachment.setBlendEnable(static_cast<vk::Bool32>(true));
        colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
        colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
        colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
        colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
        colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);
        return colorBlendAttachment;
    }

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colorBlendAttachment.setBlendEnable(static_cast<vk::Bool32>(false));
    colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
    colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);
    return colorBlendAttachment;
}

vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo()
{
    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setAttachmentCount(0);
    colorBlending.setPAttachments(nullptr);
    return colorBlending;
}

vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, bool useAlphaBlending)
{
    if (useAlphaBlending)
    {
        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(static_cast<vk::Bool32>(false));
        colorBlending.setLogicOp(vk::LogicOp::eAnd);
        colorBlending.setAttachmentCount(1);
        colorBlending.setPAttachments(&colorBlendAttachment);
        colorBlending.setBlendConstants({1.0f, 1.0f, 1.0f, 1.0f});
        return colorBlending;
    }

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(static_cast<vk::Bool32>(false));
    colorBlending.setLogicOp(vk::LogicOp::eCopy);
    colorBlending.setAttachmentCount(1);
    colorBlending.setPAttachments(&colorBlendAttachment);
    colorBlending.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
    return colorBlending;
}

auto CreatePushConstantRange(vk::ShaderStageFlags stageFlags, uint32_t dataTypeSize) -> vk::PushConstantRange
{
    return vk::PushConstantRange{stageFlags, 0u, dataTypeSize};
}

auto CreatePipelineLayoutCreateInfo() -> vk::PipelineLayoutCreateInfo
{
    return vk::PipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{}, 0u, nullptr, 0u, nullptr};
}

auto CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange) -> vk::PipelineLayoutCreateInfo
{
    return vk::PipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{}, 0u, nullptr, 1u, &pushConstantRange};
}

auto CreatePipelineLayoutCreateInfo(std::span<const vk::DescriptorSetLayout> layouts) -> vk::PipelineLayoutCreateInfo
{
    const auto layoutSize = static_cast<uint32_t>(layouts.size());
    return vk::PipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{}, layoutSize, layouts.data(), 0u, nullptr};
}

auto CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange,
                                    std::span<const vk::DescriptorSetLayout> layouts) -> vk::PipelineLayoutCreateInfo
{
    const auto layoutSize = static_cast<uint32_t>(layouts.size());
    return vk::PipelineLayoutCreateInfo{vk::PipelineLayoutCreateFlags{}, layoutSize, layouts.data(), 1u, &pushConstantRange};
}

vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(vk::Device device, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT>)
{
    vk::DescriptorSetLayoutCreateInfo setInfo{};
    setInfo.setBindingCount(static_cast<uint32_t>(layoutBindings.size()));
    setInfo.setPBindings(layoutBindings.data());

    return device.createDescriptorSetLayoutUnique(setInfo);
}

vk::UniqueDescriptorSet CreateDescriptorSet(vk::Device device, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout)
{
    vk::DescriptorSetAllocateInfo allocationInfo{};
    allocationInfo.setPNext(nullptr);
    allocationInfo.setDescriptorPool(*descriptorPool);
    allocationInfo.setDescriptorSetCount(descriptorSetCount);
    allocationInfo.setPSetLayouts(descriptorSetLayout);

    // @todo: return the vector rather than the indiviual item, don't use move in return values
    return std::move(device.allocateDescriptorSetsUnique(allocationInfo)[0]);
}

auto CreateDescriptorImageInfo(vk::Sampler sampler, vk::ImageView imageView, vk::ImageLayout layout) -> vk::DescriptorImageInfo
{
    return vk::DescriptorImageInfo{sampler, imageView, layout};
}
} // namespace nc::graphics
