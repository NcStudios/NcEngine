#include "GraphicsPipeline.h"
#include "Device.h"
#include "RenderPass.h"
#include "config/Config.h"

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
    GraphicsPipeline::GraphicsPipeline(const vulkan::Device& device, const vulkan::RenderPass& renderPass)
    : m_pipelineLayout{nullptr},
      m_pipeline{nullptr}
    {
        /***********
         * SHADERS *
         * *********/
        auto defaultShaderPath = nc::config::Get().graphics.vulkanShadersPath;

        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "vert.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "frag.spv");

        auto vertexShaderModule = GraphicsPipeline::CreateShaderModule(vertexShaderByteCode, device);
        auto fragmentShaderModule = GraphicsPipeline::CreateShaderModule(fragmentShaderByteCode, device);

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
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.setVertexBindingDescriptionCount(0);
        vertexInputInfo.setPVertexBindingDescriptions(nullptr);
        vertexInputInfo.setVertexAttributeDescriptionCount(0);
        vertexInputInfo.setPVertexAttributeDescriptions(nullptr);

        /******************
         * INPUT ASSEMBLY *
         * ****************/
        // @todo: Look into using element buffers here to resuse vertices
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssembly.setPrimitiveRestartEnable(static_cast<vk::Bool32>(false));

        /************
         * VIEWPORT *
         * **********/
        // Viewport defines the transformation from the image to the framebuffer
        auto [width, height] = device.GetSwapChainExtentDimensions();
        vk::Viewport viewport{};
        viewport.setX(0.0f);
        viewport.setY(0.0f);
        viewport.setWidth(width);
        viewport.setHeight(height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        // Scissor rectangles crop the image, discarding pixels outside of the scissor rect by the rasterizer.
        vk::Rect2D scissor{};
        scissor.setOffset({0,0});
        scissor.setExtent(device.GetSwapChainExtent());

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setPViewports(&viewport);
        viewportState.setScissorCount(1);
        viewportState.setPScissors(&scissor);

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

        m_pipelineLayout = device.GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

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
        pipelineCreateInfo.setPDynamicState(nullptr);
        pipelineCreateInfo.setLayout(m_pipelineLayout.get());
        pipelineCreateInfo.setRenderPass(renderPass.GetRenderPass()); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = device.GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;

        device.GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        device.GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    const vk::Pipeline& GraphicsPipeline::GetPipeline() const
    {
         return m_pipeline.get();
    }

    vk::ShaderModule GraphicsPipeline::CreateShaderModule(const std::vector<uint32_t>& code, const vulkan::Device& device)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size()*sizeof(uint32_t));
        createInfo.setPCode(code.data());
        vk::ShaderModule shaderModule;
        if (device.GetDevice().createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

}