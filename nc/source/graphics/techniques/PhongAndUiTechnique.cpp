#include "PhongAndUiTechnique.h"
#include "Assets.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "graphics/Graphics.h"
#include "graphics/Commands.h"
#include "graphics/Initializers.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/Swapchain.h"
#include "graphics/Base.h"
#include "graphics/VertexDescriptions.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/resources/ShaderResourceService.h"

namespace nc::graphics
{
    struct Texture;

    PhongAndUiTechnique::PhongAndUiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
    : m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_swapchain{graphics->GetSwapchainPtr()},
      m_pipeline{nullptr},
      m_pipelineLayout{nullptr}
    {
        CreatePipeline(renderPass);
    }

    PhongAndUiTechnique::~PhongAndUiTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    void PhongAndUiTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "PhongVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "PhongFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment, sizeof(PhongPushConstants)); // PushConstants
        std::array<vk::DescriptorSetLayout, 4u> descriptorLayouts
        {
            *ShaderResourceService<Texture>::Get()->GetDescriptorSetLayout(),
            *ShaderResourceService<PointLightInfo>::Get()->GetDescriptorSetLayout(),
            *ShaderResourceService<ObjectData>::Get()->GetDescriptorSetLayout(),
            *ShaderResourceService<ShadowMap>::Get()->GetDescriptorSetLayout()
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
        m_pipelineLayout = m_base->GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(shaderStages.size()); // Shader stages
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
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMulitsampleCreateInfo();
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
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
        
        m_pipeline = m_base->GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
       
        m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
        m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
    }

    bool PhongAndUiTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        (void)frameData;
        return true;
    }

    void PhongAndUiTechnique::Bind(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0, 1, ShaderResourceService<Texture>::Get()->GetDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 1, 1, ShaderResourceService<PointLightInfo>::Get()->GetDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 2, 1, ShaderResourceService<ObjectData>::Get()->GetDescriptorSet(), 0, 0);
        cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 3, 1, ShaderResourceService<ShadowMap>::Get()->GetDescriptorSet(), 0, 0);
        NC_PROFILE_END();
    }

    bool PhongAndUiTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        (void)frameData;
        return true;
    }

    void PhongAndUiTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        auto pushConstants = PhongPushConstants{};
        pushConstants.cameraPos = frameData.cameraPosition;
        cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(PhongPushConstants), &pushConstants);

        uint32_t objectInstance = 0;
        for(const auto& mesh : frameData.meshes)
        {
            cmd->drawIndexed(mesh.indexCount, 1, mesh.firstIndex, mesh.firstVertex, objectInstance); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            
            #ifdef NC_EDITOR_ENABLED
            m_graphics->IncrementDrawCallCount();
            #endif
            
            ++objectInstance;
        }

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);
        NC_PROFILE_END();
    }

    void PhongAndUiTechnique::Clear() noexcept
    {
    }
}