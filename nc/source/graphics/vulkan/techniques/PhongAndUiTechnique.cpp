#include "PhongAndUiTechnique.h"
#include "config/Config.h"
#include "component/Transform.h"
#include "component/vulkan/MeshRenderer.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/Initializers.h"
#include "graphics/vulkan/PhongMaterial.h"
#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/resources/TransformBuffer.h"
#include "graphics/vulkan/resources/ImmutableBuffer.h"
#include "graphics/vulkan/resources/ResourceManager.h"
#include "graphics/Graphics2.h"

namespace nc::graphics::vulkan
{
    PhongAndUiTechnique::PhongAndUiTechnique(nc::graphics::Graphics2* graphics)
    : TechniqueBase(TechniqueType::Simple, graphics),
      m_descriptorSetLayout{},
      m_textureDescriptors{}
    {
        m_renderPasses.push_back(m_swapchain->GetPassDefinition());
        CreatePipeline();
    }

    PhongAndUiTechnique::~PhongAndUiTechnique() noexcept
    {
        m_base->GetDevice().destroyDescriptorSetLayout(m_descriptorSetLayout);
    }

    void PhongAndUiTechnique::CreatePipeline()
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().vulkanShadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "vert.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "frag.spv");

        auto vertexShaderModule = PhongAndUiTechnique::CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = PhongAndUiTechnique::CreateShaderModule(fragmentShaderByteCode, m_base);

        vk::PipelineShaderStageCreateInfo shaderStages[] = 
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        m_descriptorSetLayout = *ResourceManager::GetDescriptorSetLayout();

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, sizeof(PushConstants)); // PushConstants
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, m_descriptorSetLayout);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayout(pipelineLayoutInfo);

        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(2); // Shader stages
        pipelineCreateInfo.setPStages(shaderStages); // Shader stages
        auto vertexBindingDescription = GetVertexBindingDescription();
        auto vertexAttributeDescription = GetVertexAttributeDescriptions();
        auto vertexInputInfo = CreateVertexInputCreateInfo(vertexBindingDescription, vertexAttributeDescription);
        pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
        auto inputAssembly = CreateInputAssemblyCreateInfo();
        pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
        auto viewportState = CreateViewportCreateInfo();
        pipelineCreateInfo.setPViewportState(&viewportState);
        auto rasterizer = CreateRasterizationCreateInfo();
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMulitsampleCreateInfo();
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlendAttachment = CreateColorBlendAttachmentCreateInfo();
        auto colorBlending = CreateColorBlendStateCreateInfo(colorBlendAttachment);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout);
        pipelineCreateInfo.setRenderPass(m_renderPasses[0]); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = m_base->GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    void PhongAndUiTechnique::Record(Commands* commands)
    {
        vk::ClearValue clearValues[2];
		clearValues[0].setColor(vk::ClearColorValue(m_graphics->GetClearColor()));
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
            m_swapchain->WaitForFrameFence(true);

            // Begin recording commands to each command buffer.
            commandBuffers[i].begin(beginInfo);
            {
                vk::RenderPassBeginInfo renderPassInfo{};
                renderPassInfo.setRenderPass(m_renderPasses[0]); // Specify the render pass and attachments.
                renderPassInfo.setFramebuffer(m_swapchain->GetFrameBuffer((uint32_t)i));
                renderPassInfo.renderArea.setOffset({0,0}); // Specify the dimensions of the render area.
                renderPassInfo.renderArea.setExtent(m_swapchain->GetExtent());
                renderPassInfo.setClearValueCount(2); // Set clear color
                renderPassInfo.setPClearValues(clearValues);

                commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
                {
                    commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
                    auto dimensions = m_graphics->GetDimensions();
                    SetViewportAndScissor(&commandBuffers[i], dimensions);

                    vk::DeviceSize offsets[] = { 0 };
                    commandBuffers[i].bindVertexBuffers(0, 1, ResourceManager::GetVertexBuffer(), offsets);
                    commandBuffers[i].bindIndexBuffer(*ResourceManager::GetIndexBuffer(), 0, vk::IndexType::eUint32);
                    commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, ResourceManager::GetDescriptorSet(), 0, 0);

                    for (auto& [meshUid, renderers] : m_meshRenderers)
                    {
                        const auto viewMatrix = m_graphics->GetViewMatrix();
                        const auto projectionMatrix = m_graphics->GetProjectionMatrix();
                        const auto meshAccessor = ResourceManager::GetMeshAccessor(meshUid);
                        
                        for (auto* meshRenderer : renderers)
                        {
                            auto& material = meshRenderer->GetMaterial();
                            auto modelViewMatrix = meshRenderer->GetTransform()->GetTransformationMatrix() * viewMatrix;
                            auto matrix = GetMatrices(modelViewMatrix, modelViewMatrix * projectionMatrix);
                            
                            auto pushConstants = PushConstants{};
                            pushConstants.model = matrix.model;
                            pushConstants.modelView = matrix.modelView;
                            pushConstants.baseColorIndex = ResourceManager::GetTextureAccessor(material.baseColor);
                            pushConstants.normalColorIndex = ResourceManager::GetTextureAccessor(material.normal);
                            pushConstants.roughnessColorIndex = ResourceManager::GetTextureAccessor(material.roughness);

                            commandBuffers[i].pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstants), &pushConstants);
                            commandBuffers[i].drawIndexed(meshAccessor.indicesCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
                        }
                    }
                }

                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[i]);

                commandBuffers[i].endRenderPass();
            }
            commandBuffers[i].end();
        }
    }
}