#ifdef NC_EDITOR_ENABLED
#include "WireframeTechnique.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "ecs/component/Transform.h"
#include "ecs/Registry.h"
#include "graphics/Base.h"
#include "graphics/DebugRenderer.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/Swapchain.h"
#include "graphics/VertexDescriptions.h"

#include <iostream>

namespace nc::graphics
{
    WireframeTechnique::WireframeTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
    : m_graphics{graphics},
      m_base{graphics->GetBasePtr()},
      m_meshPath{"project/assets/mesh/cube.nca"},
      m_pipeline{nullptr},
      m_pipelineLayout{nullptr}
    {
        CreatePipeline(renderPass);
        LoadMeshAsset(m_meshPath);
    }

    WireframeTechnique::~WireframeTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool WireframeTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        return frameData.colliderDebugWidget.has_value() 
        #ifndef NC_DEBUG_RENDERING
        ;
        #else
        || !m_graphics->GetDebugData()->points.empty()
        || !m_graphics->GetDebugData()->planes.empty()
        || !m_graphics->GetDebugData()->lines.empty();
        #endif
    }

    void WireframeTechnique::Bind(vk::CommandBuffer* cmd)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        NC_PROFILE_END();
    }

    void WireframeTechnique::CreatePipeline(vk::RenderPass* renderPass)
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "WireframeVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "WireframeFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
        auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(WireframePushConstants)); // PushConstants
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange);
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
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eLine, 2.0f);
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

    bool WireframeTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        return frameData.colliderDebugWidget.has_value() 
        #ifndef NC_DEBUG_RENDERING
        ;
        #else
        || !m_graphics->GetDebugData()->points.empty()
        || !m_graphics->GetDebugData()->planes.empty()
        || !m_graphics->GetDebugData()->lines.empty();
        #endif
    }

    void WireframeTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);

        auto pushConstants = WireframePushConstants{};

        if (frameData.colliderDebugWidget.has_value())
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = frameData.colliderDebugWidget->transformationMatrix;
            const auto meshAccessor = AssetService<MeshView>::Get()->Acquire(frameData.colliderDebugWidget->meshUid);
            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(meshAccessor.indexCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        #ifdef NC_DEBUG_RENDERING
        const auto debugMeshAccessor = AssetService<MeshView>::Get()->Acquire(m_meshPath);

        for (const auto& point : m_graphics->GetDebugData()->points)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = point;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        for (const auto& line : m_graphics->GetDebugData()->lines)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = line;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        for (const auto& plane : m_graphics->GetDebugData()->planes)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = plane;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }
        #endif

        NC_PROFILE_END();
    }
}
#endif