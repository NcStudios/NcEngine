#include "Initializers.h"
#include "graphics/Base.h"
#include "graphics/MeshManager.h"
#include "graphics/Graphics.h"

namespace nc::graphics
{
    vk::SamplerCreateInfo CreateSampler(vk::SamplerAddressMode addressMode)
    {
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.setMagFilter(vk::Filter::eLinear);
        samplerInfo.setMinFilter(vk::Filter::eLinear);
        samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
        samplerInfo.setAddressModeU(addressMode);
        samplerInfo.setAddressModeV(addressMode);
        samplerInfo.setAddressModeW(addressMode);
        samplerInfo.setAnisotropyEnable(VK_TRUE);
        samplerInfo.setMaxAnisotropy(1.0f);
        samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
        samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
        samplerInfo.setCompareEnable(VK_FALSE);
        samplerInfo.setCompareOp(vk::CompareOp::eAlways);
        samplerInfo.setMipLodBias(0.0f);
        samplerInfo.setMinLod(0.0f);
        samplerInfo.setMaxLod(0.0f);

        return samplerInfo;
    }

    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp)
    {
        vk::AttachmentDescription attachmentDescription{};
        attachmentDescription.setFormat(format);
        attachmentDescription.setSamples(vk::SampleCountFlagBits::e1);
        attachmentDescription.setLoadOp(loadOp);
        attachmentDescription.setStoreOp(storeOp);
        attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);

        switch (type)
        {
            case AttachmentType::Color:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
                break;

            case AttachmentType::Depth:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eClear);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                break;
            case AttachmentType::ShadowDepth:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setFinalLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
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
            case AttachmentType::ShadowDepth:
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

    vk::SubpassDescription CreateSubpassDescription(const vk::AttachmentReference& depthReference)
    {
        vk::SubpassDescription subpassDescription{};
        subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
        subpassDescription.setColorAttachmentCount(0);
        subpassDescription.setPDepthStencilAttachment(&depthReference);
        return subpassDescription;
    }

    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask,  vk::AccessFlags destAccessMask, vk::DependencyFlags dependencyFlags)
    {
        vk::SubpassDependency subpassDependency = CreateSubpassDependency(sourceSubpassIndex, destSubpassIndex, sourceStageMask, destStageMask, sourceAccessMask, destAccessMask);
        subpassDependency.setDependencyFlags(dependencyFlags);
        return subpassDependency;      
    }

    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask, vk::AccessFlags destAccessMask)
    {
        vk::SubpassDependency subpassDependency;

        subpassDependency.setSrcSubpass(sourceSubpassIndex); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
        subpassDependency.setDstSubpass(destSubpassIndex); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
        subpassDependency.setSrcStageMask(sourceStageMask); // The type of operation to wait on. (What our dependency is)
        subpassDependency.setDstStageMask(destStageMask); // Specifies the type of operation that should do the waiting
        subpassDependency.setSrcAccessMask(sourceAccessMask);  // Specifies the specific operation that should do the waiting
        subpassDependency.setDstAccessMask(destAccessMask);  // Specifies the specific operation that should do the waiting
        
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

    vk::PushConstantRange CreatePushConstantRange(vk::ShaderStageFlags stageFlags, size_t dataTypeSize)
    {
        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.setStageFlags(stageFlags);
        pushConstantRange.setOffset(0);
        pushConstantRange.setSize(dataTypeSize);
        return pushConstantRange;
    }

    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange, const std::vector<vk::DescriptorSetLayout>& layouts)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(layouts.size());  
        pipelineLayoutInfo.setPSetLayouts(layouts.data());  
        pipelineLayoutInfo.setPushConstantRangeCount(1); 
        pipelineLayoutInfo.setPPushConstantRanges(&pushConstantRange);
        return pipelineLayoutInfo;
    }

    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const std::vector<vk::DescriptorSetLayout>& layouts)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(layouts.size());  
        pipelineLayoutInfo.setPSetLayouts(layouts.data());  
        pipelineLayoutInfo.setPushConstantRangeCount(0); 
        pipelineLayoutInfo.setPPushConstantRanges(nullptr);
        return pipelineLayoutInfo;
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

    vk::DescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, 
                                                                    uint32_t descriptorCount, 
                                                                    vk::DescriptorType type, 
                                                                    vk::ShaderStageFlags shaderStages)
    {
        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.setBinding(binding);
        layoutBinding.setDescriptorCount(descriptorCount);
        layoutBinding.setDescriptorType(type);
        layoutBinding.setPImmutableSamplers(nullptr);
        layoutBinding.setStageFlags(shaderStages);
        return layoutBinding;
    }

    vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(Graphics* graphics, std::vector<vk::DescriptorSetLayoutBinding> layoutBindings, vk::DescriptorBindingFlagsEXT bindingFlags)
    {
        vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{};
        extendedInfo.setPNext(nullptr);
        extendedInfo.setBindingCount(layoutBindings.size());
        extendedInfo.setPBindingFlags(&bindingFlags);

        vk::DescriptorSetLayoutCreateInfo setInfo{};
        setInfo.setBindingCount(layoutBindings.size());
        setInfo.setFlags(vk::DescriptorSetLayoutCreateFlags());
        setInfo.setPNext(&extendedInfo);
        setInfo.setPBindings(layoutBindings.data());

        return graphics->GetBasePtr()->GetDevice().createDescriptorSetLayoutUnique(setInfo);
    }
    
    vk::UniqueDescriptorSet CreateDescriptorSet(Graphics* graphics, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout)
    {
        vk::DescriptorSetAllocateInfo allocationInfo{};
        allocationInfo.setPNext(nullptr);
        allocationInfo.setDescriptorPool(*descriptorPool);
        allocationInfo.setDescriptorSetCount(descriptorSetCount);
        allocationInfo.setPSetLayouts(descriptorSetLayout);

        // @todo: return the vector rather than the indiviual item, don't use move in return values
        return std::move(graphics->GetBasePtr()->GetDevice().allocateDescriptorSetsUnique(allocationInfo)[0]);
    }

    vk::WriteDescriptorSet CreateSamplerDescriptorWrite(vk::Sampler* sampler, vk::DescriptorSet* descriptorSet, uint32_t binding)
    {
        vk::WriteDescriptorSet write;
        vk::DescriptorImageInfo samplerInfo = {};
        samplerInfo.sampler = *sampler;

        write.setDstBinding(binding);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eSampler);
        write.setDescriptorCount(1);
        write.setDstSet(*descriptorSet);
        write.setPBufferInfo(0);
        write.setPImageInfo(&samplerInfo);
        return write;
    }

    vk::WriteDescriptorSet CreateImagesDescriptorWrite(vk::DescriptorSet* descriptorSet, std::vector<vk::DescriptorImageInfo>* imagesInfo, uint32_t imagesCount, uint32_t binding)
    {
        vk::WriteDescriptorSet write;

        imagesInfo->reserve(imagesCount);

        write.setDstBinding(binding);
        write.setDstArrayElement(0);
        write.setDescriptorType(vk::DescriptorType::eSampledImage);
        write.setDescriptorCount(imagesCount);
        write.setDstSet(*descriptorSet);
        write.setPBufferInfo(0);
        write.setPImageInfo(imagesInfo->data());
        return write;
    }

    vk::DescriptorImageInfo CreateDescriptorImageInfo(vk::Sampler* sampler, const vk::ImageView& imageView, vk::ImageLayout layout)
    {
        vk::DescriptorImageInfo imageInfo = {};
        imageInfo.setSampler(*sampler);
        imageInfo.setImageView(imageView);
        imageInfo.setImageLayout(layout);
        return imageInfo;
    }
}