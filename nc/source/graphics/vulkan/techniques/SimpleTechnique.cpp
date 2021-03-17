#include "SimpleTechnique.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/resources/VertexBuffer.h"
#include "graphics/vulkan/FrameManager.h"
#include "graphics/vulkan/resources/VertexBuffer.h"
#include "graphics/vulkan/techniques/TechniqueType.h"
#include "graphics/Graphics2.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "config/Config.h"

namespace nc::graphics::vulkan
{
    SimpleTechnique::SimpleTechnique(vulkan::FrameManager* frameManager)
    : TechniqueBase(TechniqueType::Simple, frameManager)
    {
        CreateRenderPasses();
        CreatePipeline();
    }

    void SimpleTechnique::CreateRenderPasses()
    {
        std::array<vk::AttachmentDescription, 2> attachments = {};

        // Color attachment
        attachments[0].setFormat(m_swapchain.GetFormat());
        attachments[0].setSamples(vk::SampleCountFlagBits::e1);
        attachments[0].setLoadOp(vk::AttachmentLoadOp::eClear);
        attachments[0].setStoreOp(vk::AttachmentStoreOp::eStore);
        attachments[0].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments[0].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[0].setInitialLayout(vk::ImageLayout::eUndefined);
        attachments[0].setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        // Depth attachment
        attachments[1].setFormat(m_base.GetDepthFormat());
        attachments[1].setSamples(vk::SampleCountFlagBits::e1);
        attachments[1].setLoadOp(vk::AttachmentLoadOp::eClear);
        attachments[1].setStoreOp(vk::AttachmentStoreOp::eStore);
        attachments[1].setStencilLoadOp(vk::AttachmentLoadOp::eClear);
        attachments[1].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[1].setInitialLayout(vk::ImageLayout::eUndefined);
        attachments[1].setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::AttachmentReference colorReference = {};
        colorReference.setAttachment(0);
        colorReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentReference depthReference = {};
        depthReference.setAttachment(1);
        depthReference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        // Subpass
        vk::SubpassDescription subpass{};
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
        subpass.setColorAttachmentCount(1);
        subpass.setPColorAttachments(&colorReference);
        subpass.setPDepthStencilAttachment(&depthReference);
        subpass.setInputAttachmentCount(0);
        subpass.setPreserveAttachmentCount(0);
        subpass.setPPreserveAttachments(nullptr);
        subpass.setPResolveAttachments(nullptr);

        // Subpass dependencies
        std::array<vk::SubpassDependency, 2> dependencies{};
        dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
        dependencies[0].setDstSubpass(0); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
        dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // The type of operation to wait on. (What our dependency is)
        dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // Specifies the type of operation that should do the waiting
        dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
        dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
        dependencies[0].setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting

        dependencies[1].setSrcSubpass(0); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
        dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
        dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // The type of operation to wait on. (What our dependency is)
        dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // Specifies the type of operation that should do the waiting
        dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
        dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
        dependencies[1].setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting

        // Render pass info
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
        renderPassInfo.setPAttachments(attachments.data());
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpass);
        renderPassInfo.setDependencyCount(static_cast<uint32_t>(dependencies.size()));
        renderPassInfo.setPDependencies(dependencies.data());

        vk::RenderPass renderPass;
        if (m_base.GetDevice().createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Could not create render pass.");
        }

        m_renderPasses.emplace_back(renderPass);
    }

    void SimpleTechnique::CreatePipeline()
    {
        // Shaders
        auto defaultShaderPath = nc::config::Get().graphics.vulkanShadersPath;

        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "vert.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "frag.spv");

        auto vertexShaderModule = SimpleTechnique::CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = SimpleTechnique::CreateShaderModule(fragmentShaderByteCode, m_base);

        vk::PipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
        vertexShaderStageInfo.setModule(vertexShaderModule);
        vertexShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
        fragmentShaderStageInfo.setModule(fragmentShaderModule);
        fragmentShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

        // Vertex input format
        auto vertexBindingDescription = GetVertexBindingDescription();
        auto vertexAttributeDescription = GetVertexAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(1);
        vertexInputInfo.setPVertexBindingDescriptions(&vertexBindingDescription);
        vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributeDescription.size()));
        vertexInputInfo.setPVertexAttributeDescriptions(vertexAttributeDescription.data());

        // Input assembly
        // @todo: Look into using element buffers here to reuse vertices
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssembly.setPrimitiveRestartEnable(static_cast<vk::Bool32>(false));

        // Vieport
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setPViewports(nullptr);
        viewportState.setScissorCount(1);
        viewportState.setPScissors(nullptr);

        // Rasterizer
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

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(static_cast<vk::Bool32>(false));
        multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling.setMinSampleShading(1.0f);
        multisampling.setPSampleMask(nullptr);
        multisampling.setAlphaToCoverageEnable(static_cast<vk::Bool32>(false));
        multisampling.setAlphaToOneEnable(static_cast<vk::Bool32>(false));

        // Depth testing
        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.setDepthTestEnable(static_cast<vk::Bool32>(true));
        depthStencil.setDepthWriteEnable(static_cast<vk::Bool32>(true));
        depthStencil.setDepthCompareOp(vk::CompareOp::eLessOrEqual);

        // Color blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        colorBlendAttachment.setBlendEnable(static_cast<vk::Bool32>(false));
        colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOne);
        colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
        colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
        colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
        colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(static_cast<vk::Bool32>(false));
        colorBlending.setLogicOp(vk::LogicOp::eCopy);
        colorBlending.setAttachmentCount(1);
        colorBlending.setPAttachments(&colorBlendAttachment);
        colorBlending.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

        // PipelineLayout
        // Can use uniform values to bind texture samplers, buffers, etc to the shader here.
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(0);  
        pipelineLayoutInfo.setPSetLayouts(nullptr);  
        pipelineLayoutInfo.setPushConstantRangeCount(0); 
        pipelineLayoutInfo.setPPushConstantRanges(nullptr);  

        m_pipelineLayout = m_base.GetDevice().createPipelineLayout(pipelineLayoutInfo);

        // Dynamic state
        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(2); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages); // Shader stages
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        pipelineCreateInfo.setPViewportState(&viewportState);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout);
        pipelineCreateInfo.setRenderPass(m_renderPasses[0]); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = m_base.GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;

        m_base.GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base.GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    void SimpleTechnique::Record()
    {
        vk::ClearValue clearValues[2];
		clearValues[0].setColor(vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})));
		clearValues[1].setDepthStencil({ 1.0f, 0 });

        auto& commandBuffers = *m_commands->GetCommandBuffers();

        // Begin recording on each of the command buffers.
        for (size_t i = 0; i < commandBuffers.size(); ++i)
        {
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.setPInheritanceInfo(nullptr);

            // Begin recording commands to each command buffer.
            commandBuffers[i].begin(beginInfo);
            {
                vk::RenderPassBeginInfo renderPassInfo{};
                renderPassInfo.setRenderPass(m_renderPasses[0]); // Specify the render pass and attachments.
                renderPassInfo.setFramebuffer(m_swapchain.GetFrameBuffer((uint32_t)i));
                renderPassInfo.renderArea.setOffset({0,0}); // Specify the dimensions of the render area.
                renderPassInfo.renderArea.setExtent(m_swapchain.GetExtent());
                renderPassInfo.setClearValueCount(2); // Set clear color
                renderPassInfo.setPClearValues(clearValues);

                commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
                {
                    commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);

                    // Bind vertex buffers (combined into one big one from frame manager) for the models
                    auto vertexBuffer = m_frameManager->GetVertexBuffer(TechniqueType::Simple);
                    auto indexBuffer = m_frameManager->GetIndexBuffer(TechniqueType::Simple);
                    vk::Buffer vertexBuffers[] = { vertexBuffer.GetBuffer() };
                    vk::DeviceSize offsets[] = { 0 };
                    
                    auto dimensions = graphics::d3dresource::GraphicsResourceManager::GetGraphics2()->GetDimensions();
                    vk::Viewport viewport = {};
                    viewport.setWidth(dimensions.x);
                    viewport.setHeight(dimensions.y);
                    viewport.setMinDepth(0.0f);
                    viewport.setMaxDepth(1.0f);

                    vk::Extent2D extent = {};
                    extent.setWidth(dimensions.x);
                    extent.setHeight(dimensions.y);
                    vk::Rect2D scissor = {};
                    scissor.setExtent(extent);
                    scissor.setOffset({0, 0});

                    commandBuffers[i].setViewport(0, 1, &viewport);
                    commandBuffers[i].setScissor(0, 1, &scissor);
                    commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
                    commandBuffers[i].bindIndexBuffer(indexBuffer.GetBuffer(), 0, vk::IndexType::eUint32);

                    commandBuffers[i].drawIndexed(static_cast<uint32_t>(indexBuffer.GetIndices().size()), 1, 0, 0, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                }
                commandBuffers[i].endRenderPass();
            }
            commandBuffers[i].end();
        }
    }
}