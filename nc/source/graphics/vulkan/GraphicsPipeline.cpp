#include "GraphicsPipeline.h"
#include "Base.h"
#include "config/Config.h"
#include "VertexBuffer.h"
#include "Swapchain.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

namespace
{
    std::vector<uint32_t> ReadShader(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open() || file.tellg() == -1)
        {
            throw std::runtime_error("Failed to open file.");
        }
        auto fileSize = static_cast<uint32_t>(file.tellg());
        if (fileSize % 4 != 0)
        {
            throw std::runtime_error("The file of shader byte code was not uint32_t aligned.");
        }

        auto bufferSize = fileSize/sizeof(uint32_t);
        std::vector<uint32_t> buffer(bufferSize);
        auto charBuffer = reinterpret_cast<char*>(buffer.data());
        file.seekg(0);
        file.read(charBuffer, fileSize);
        if (file.fail())
        {
            file.close();
            throw std::runtime_error("The file was wonky. (failbit set)");
        }
        file.close();
        return buffer;
    }
}

namespace nc::graphics::vulkan
{
    GraphicsPipeline::GraphicsPipeline(const vulkan::Base& base, const vulkan::Swapchain& swapchain)
    : m_pipelineLayout{nullptr},
      m_pipeline{nullptr}
    {
        /***********
         * SHADERS *
         * *********/
        auto defaultShaderPath = nc::config::Get().graphics.vulkanShadersPath;

        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "vert.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "frag.spv");

        auto vertexShaderModule = GraphicsPipeline::CreateShaderModule(vertexShaderByteCode, base);
        auto fragmentShaderModule = GraphicsPipeline::CreateShaderModule(fragmentShaderByteCode, base);

        vk::PipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
        vertexShaderStageInfo.setModule(vertexShaderModule);
        vertexShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
        fragmentShaderStageInfo.setModule(fragmentShaderModule);
        fragmentShaderStageInfo.setPName("main");

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

        /***********************
         * VERTEX INPUT FORMAT *
         * *********************/
        // Set up the vertex data format. Instancing would be configured here.
        // @todo: Instancing
        auto vertexBindingDescription = vertex::GetVertexBindingDescription();
        auto vertexAttributeDescription = vertex::GetVertexAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(1);
        vertexInputInfo.setPVertexBindingDescriptions(&vertexBindingDescription);
        vertexInputInfo.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexAttributeDescription.size()));
        vertexInputInfo.setPVertexAttributeDescriptions(vertexAttributeDescription.data());

        /******************
         * INPUT ASSEMBLY *
         * ****************/
        // @todo: Look into using element buffers here to reuse vertices
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssembly.setPrimitiveRestartEnable(static_cast<vk::Bool32>(false));

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setPViewports(nullptr);
        viewportState.setScissorCount(1);
        viewportState.setPScissors(nullptr);

        /*************
         * RASTERIZER *
         * ***********/
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

        /*****************
         * MULTISAMPLING *
         * ***************/
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(static_cast<vk::Bool32>(false));
        multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampling.setMinSampleShading(1.0f);
        multisampling.setPSampleMask(nullptr);
        multisampling.setAlphaToCoverageEnable(static_cast<vk::Bool32>(false));
        multisampling.setAlphaToOneEnable(static_cast<vk::Bool32>(false));

        /*****************************
         * DEPTH AND STENCIL TESTING *
         * ***************************/
        // @todo: Add depth and stencil testing, not needed for basic triangle render.

        /******************
         * COLOR BLENDING *
         * ****************/
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

        /*******************
         * PIPELINE LAYOUT *
         * *****************/
        // Can use uniform values to bind texture samplers, buffers, etc to the shader here.
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setSetLayoutCount(0);  
        pipelineLayoutInfo.setPSetLayouts(nullptr);  
        pipelineLayoutInfo.setPushConstantRangeCount(0); 
        pipelineLayoutInfo.setPPushConstantRanges(nullptr);  

        m_pipelineLayout = base.GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

        /*****************
         * DYNAMIC STATE *
         * ***************/
        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        /*******************
         * GRAPHICS PIPELINE *
         * *****************/
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(2); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages); // Shader stages
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        pipelineCreateInfo.setPViewportState(&viewportState);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        pipelineCreateInfo.setPDepthStencilState(nullptr);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout.get());
        pipelineCreateInfo.setRenderPass(swapchain.GetFrameBufferFillPass()); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = base.GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;

        base.GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        base.GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    const vk::Pipeline& GraphicsPipeline::GetPipeline() const
    {
         return m_pipeline.get();
    }

    vk::ShaderModule GraphicsPipeline::CreateShaderModule(const std::vector<uint32_t>& code, const vulkan::Base& base)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size()*sizeof(uint32_t));
        createInfo.setPCode(code.data());
        vk::ShaderModule shaderModule;
        if (base.GetDevice().createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

}