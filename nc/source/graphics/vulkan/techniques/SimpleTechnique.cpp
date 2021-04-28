#include "SimpleTechnique.h"
#include "config/Config.h"
#include "component/Transform.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Initializers.h"
#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/Resources/TransformBuffer.h"
#include "graphics/vulkan/Resources/ImmutableBuffer.h"
#include "graphics/Graphics2.h"

namespace nc::graphics::vulkan
{
    SimpleTechnique::SimpleTechnique(const GlobalData& globalData, nc::graphics::Graphics2* graphics)
    : TechniqueBase(TechniqueType::Simple, globalData, graphics),
      m_descriptorSetLayout{}
    {
        CreateRenderPasses();
        CreatePipeline();
    }

    SimpleTechnique::~SimpleTechnique()
    {
        m_base.GetDevice().destroyDescriptorSetLayout(m_descriptorSetLayout);
    }

    void SimpleTechnique::CreateRenderPasses()
    {
        std::array<vk::AttachmentDescription, 2> attachments = 
        {
            CreateAttachmentDescription(AttachmentType::Color, m_swapchain.GetFormat()),
            CreateAttachmentDescription(AttachmentType::Depth, m_base.GetDepthFormat()),
        };

        vk::AttachmentReference colorReference = CreateAttachmentReference(AttachmentType::Color, 0);
        vk::AttachmentReference depthReference = CreateAttachmentReference(AttachmentType::Depth, 1);
        vk::SubpassDescription subpass = CreateSubpassDescription(colorReference, depthReference);

        std::array<vk::SubpassDependency, 2> dependencies =
        { 
            CreateSubpassDependency(AttachmentType::Color),
            CreateSubpassDependency(AttachmentType::Depth)
        };

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
        auto defaultShaderPath = nc::config::GetGraphicsSettings().vulkanShadersPath;

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

        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eVertex);
        pushConstantRange.setOffset(0);
        pushConstantRange.setSize(sizeof(TransformMatrices));

        // PipelineLayout
        // Can use uniform values to bind texture samplers, buffers, etc to the shader here.
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(0);  
        pipelineLayoutInfo.setPSetLayouts(nullptr);  
        pipelineLayoutInfo.setPushConstantRangeCount(1); 
        pipelineLayoutInfo.setPPushConstantRanges(&pushConstantRange);  

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

    void SimpleTechnique::Record(Commands* commands)
    {
        vk::ClearValue clearValues[2];
		clearValues[0].setColor(vk::ClearColorValue(std::array<float, 4>({{0.2f, 0.2f, 0.2f, 0.2f}})));
		clearValues[1].setDepthStencil({ 1.0f, 0 });

        auto& commandBuffers = *commands->GetCommandBuffers();
        auto& dimensions = m_graphics->GetDimensions();

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

        // Begin recording on each of the command buffers.
        for (size_t i = 0; i < commandBuffers.size(); ++i)
        {
            vk::CommandBufferBeginInfo beginInfo;
            beginInfo.setPInheritanceInfo(nullptr);
            m_swapchain.WaitForFrameFence(true);

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
                    commandBuffers[i].setViewport(0, 1, &viewport);
                    commandBuffers[i].setScissor(0, 1, &scissor);

                    vk::DeviceSize offsets[] = { 0 };
                    commandBuffers[i].bindVertexBuffers(0, 1, m_globalData.vertexBuffer, offsets);
                    commandBuffers[i].bindIndexBuffer(*m_globalData.indexBuffer, 0, vk::IndexType::eUint32);

                    for (auto& mesh : m_meshes)
                    {
                        const auto viewMatrix = m_graphics->GetViewMatrix();
                        const auto projectionMatrix = m_graphics->GetProjectionMatrix();
                        
                        // Bind the transforms per object and draw each object
                        for (const auto* transform : m_objects.at(mesh.uid))
                        {
                            auto modelViewMatrix = transform->GetTransformationMatrix() * viewMatrix;
                            auto matrix = GetMatrices(modelViewMatrix, modelViewMatrix * projectionMatrix);
                            commandBuffers[i].pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(TransformMatrices), &matrix);
                            commandBuffers[i].drawIndexed(mesh.indicesCount, 1, mesh.firstIndex, mesh.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                        }
                    }
                }
                commandBuffers[i].endRenderPass();
            }
            commandBuffers[i].end();
        }
    }
}