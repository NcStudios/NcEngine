#include "Initializers.h"
#include "graphics/vulkan/MeshManager.h"

namespace nc::graphics::vulkan
{
    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format)
    {
        vk::AttachmentDescription attachmentDescription{};
        attachmentDescription.setFormat(format);
        attachmentDescription.setSamples(vk::SampleCountFlagBits::e1);
        attachmentDescription.setLoadOp(vk::AttachmentLoadOp::eClear);
        attachmentDescription.setStoreOp(vk::AttachmentStoreOp::eStore);
        attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);

        switch (type)
        {
            case AttachmentType::Color:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);
                attachmentDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
                break;

            case AttachmentType::Depth:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eClear);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                break;
        }
        return attachmentDescription;
    }

    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex)
    {
        vk::AttachmentReference attachmentReference;
        attachmentReference.setAttachment(attachmentIndex);

        switch (type)
        {
            case AttachmentType::Color:
                attachmentReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
                break;

            case AttachmentType::Depth:
                attachmentReference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                break;
        }
        return attachmentReference;
    }

    vk::SubpassDescription CreateSubpassDescription(const vk::AttachmentReference& colorReference, const vk::AttachmentReference& depthReference)
    {
        vk::SubpassDescription subpassDescription{};
        subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
        subpassDescription.setColorAttachmentCount(1);
        subpassDescription.setPColorAttachments(&colorReference);
        subpassDescription.setPDepthStencilAttachment(&depthReference);
        subpassDescription.setInputAttachmentCount(0);
        subpassDescription.setPreserveAttachmentCount(0);
        subpassDescription.setPPreserveAttachments(nullptr);
        subpassDescription.setPResolveAttachments(nullptr);
        return subpassDescription;
    }

    vk::SubpassDependency CreateSubpassDependency(AttachmentType type)
    {
        vk::SubpassDependency subpassDependency;

        switch (type)
        {
            case AttachmentType::Color:
                subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
                subpassDependency.setDstSubpass(0); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
                subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // The type of operation to wait on. (What our dependency is)
                subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // Specifies the type of operation that should do the waiting
                subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting
                break;

            case AttachmentType::Depth:
                subpassDependency.setSrcSubpass(0); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
                subpassDependency.setDstSubpass(VK_SUBPASS_EXTERNAL); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
                subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // The type of operation to wait on. (What our dependency is)
                subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // Specifies the type of operation that should do the waiting
                subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting
                break;
        }
        
        return subpassDependency;      
    }

    vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader)
    {
        vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{};

        switch (stage)
        {
            case ShaderStage::Vertex:
                pipelineShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eVertex);
                break;
            case ShaderStage::Pixel:
                pipelineShaderStageCreateInfo.setStage(vk::ShaderStageFlagBits::eFragment);
                break;
        }

        pipelineShaderStageCreateInfo.setModule(shader);
        pipelineShaderStageCreateInfo.setPName("main");
        return pipelineShaderStageCreateInfo;
    }

    vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, const std::array<vk::VertexInputAttributeDescription, 5>& vertexAttributeDesc)
    {

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(1);
        vertexInputInfo.setPVertexBindingDescriptions(&vertexInputDesc);
        vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributeDesc.size()));
        vertexInputInfo.setPVertexAttributeDescriptions(vertexAttributeDesc.data());
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

    vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo()
    {
        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.setDepthClampEnable(static_cast<vk::Bool32>(false)); // Set to false for shadow mapping, requires enabling a GPU feature.
        rasterizer.setRasterizerDiscardEnable(static_cast<vk::Bool32>(false));
        rasterizer.setPolygonMode(vk::PolygonMode::eFill); // Set to line for wireframe, requires enabling a GPU feature.
        rasterizer.setLineWidth(1.0f); // Setting wider requires enabling the wideLines GPU feature.
        rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
        rasterizer.setFrontFace(vk::FrontFace::eClockwise);
        rasterizer.setDepthBiasEnable(static_cast<vk::Bool32>(false));
        rasterizer.setDepthBiasConstantFactor(0.0f);
        rasterizer.setDepthBiasClamp(0.0f);
        rasterizer.setDepthBiasSlopeFactor(0.0f);
        return rasterizer;
    }

    vk::PipelineMultisampleStateCreateInfo CreateMulitsampleCreateInfo()
    {
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(static_cast<vk::Bool32>(false));
        multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling.setMinSampleShading(1.0f);
        multisampling.setPSampleMask(nullptr);
        multisampling.setAlphaToCoverageEnable(static_cast<vk::Bool32>(false));
        multisampling.setAlphaToOneEnable(static_cast<vk::Bool32>(false));
        return multisampling;
    }

    vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo()
    {
        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.setDepthTestEnable(static_cast<vk::Bool32>(true));
        depthStencil.setDepthWriteEnable(static_cast<vk::Bool32>(true));
        depthStencil.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
        return depthStencil;
    }

    vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo()
    {
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

    vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment)
    {
        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(static_cast<vk::Bool32>(false));
        colorBlending.setLogicOp(vk::LogicOp::eCopy);
        colorBlending.setAttachmentCount(1);
        colorBlending.setPAttachments(&colorBlendAttachment);
        colorBlending.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
        return colorBlending;
    }

    vk::PushConstantRange CreatePushConstantRange(ShaderStage stage, size_t dataTypeSize)
    {
        vk::PushConstantRange pushConstantRange{};

        switch (stage)
        {
            case ShaderStage::Vertex:
                pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eVertex);
                break;
            case ShaderStage::Pixel:
                pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eFragment);
                break;
        }

        pushConstantRange.setOffset(0);
        pushConstantRange.setSize(dataTypeSize);
        return pushConstantRange;
    }

    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(0);  
        pipelineLayoutInfo.setPSetLayouts(nullptr);  
        pipelineLayoutInfo.setPushConstantRangeCount(1); 
        pipelineLayoutInfo.setPPushConstantRanges(&pushConstantRange);
        return pipelineLayoutInfo;
    }
    
    vk::Viewport CreateViewport(const Vector2& dimensions)
    {
        vk::Viewport viewport = {};
        viewport.setWidth(dimensions.x);
        viewport.setHeight(dimensions.y);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);
        return viewport;
    }

    vk::Extent2D CreateExtent(const Vector2& dimensions)
    {
        vk::Extent2D extent = {};
        extent.setWidth(dimensions.x);
        extent.setHeight(dimensions.y);
        return extent;
    }

    vk::Rect2D CreateScissor(const vk::Extent2D& extent)
    {
        vk::Rect2D scissor = {};
        scissor.setExtent(extent);
        scissor.setOffset({0, 0});
        return scissor;
    }

    void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const Vector2& dimensions)
    {
         auto viewport = CreateViewport(dimensions);
        auto extent = CreateExtent(dimensions);
        auto scissor = CreateScissor(extent);
        commandBuffer->setViewport(0, 1, &viewport);
        commandBuffer->setScissor(0, 1, &scissor);
    }
}