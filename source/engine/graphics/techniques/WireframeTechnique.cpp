#ifdef NC_EDITOR_ENABLED
#include "WireframeTechnique.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "graphics/GpuOptions.h"
#include "graphics/DebugRenderer.h"
#include "graphics/Graphics.h"
#include "graphics/vk/Initializers.h"
#include "graphics/resources/ImmutableBuffer.h"
#include "graphics/ShaderUtilities.h"
#include "graphics/VertexDescriptions.h"

namespace nc::graphics
{
    WireframeTechnique::WireframeTechnique(vk::Device device, Graphics* graphics, vk::RenderPass* renderPass)
        : m_graphics{graphics},
          m_meshPath{"cube.nca"},
          m_pipeline{nullptr},
          m_pipelineLayout{nullptr}
    {
        // Shaders
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "WireframeVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "WireframeFragment.spv");

        auto vertexShaderModule = CreateShaderModule(device, vertexShaderByteCode);
        auto fragmentShaderModule = CreateShaderModule(device, fragmentShaderByteCode);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(WireframePushConstants)); // PushConstants
        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange);
        m_pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

        std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicStateInfo.setDynamicStates(dynamicStates);

        // Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.setStageCount(static_cast<uint32_t>(shaderStages.size())); // Shader stages
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
        auto multisampling = CreateMultisampleCreateInfo(graphics->GetGpuOptions()->GetMaxSamplesCount());
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

        m_pipeline = device.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;
        device.destroyShaderModule(vertexShaderModule, nullptr);
        device.destroyShaderModule(fragmentShaderModule, nullptr);

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
        #ifndef NC_DEBUG_RENDERING_ENABLED
        ;
        #else
        || !m_graphics->GetDebugData()->points.empty()
        || !m_graphics->GetDebugData()->planes.empty()
        || !m_graphics->GetDebugData()->lines.empty();
        #endif
    }

    void WireframeTechnique::Bind(vk::CommandBuffer* cmd)
    {
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
    }

    bool WireframeTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        return frameData.colliderDebugWidget.has_value() 
        #ifndef NC_DEBUG_RENDERING_ENABLED
        ;
        #else
        || !m_graphics->GetDebugData()->points.empty()
        || !m_graphics->GetDebugData()->planes.empty()
        || !m_graphics->GetDebugData()->lines.empty();
        #endif
    }

    void WireframeTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        auto pushConstants = WireframePushConstants{};

        if (frameData.colliderDebugWidget.has_value())
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = frameData.colliderDebugWidget->transformationMatrix;
            const auto meshAccessor = AssetService<MeshView>::Get()->Acquire(frameData.colliderDebugWidget->meshUid);
            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(meshAccessor.indexCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        #ifdef NC_DEBUG_RENDERING_ENABLED
        const auto debugMeshAccessor = AssetService<MeshView>::Get()->Acquire(m_meshPath);
        auto* debugData = m_graphics->GetDebugData();

        for (const auto& point : debugData->points)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = point;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        for (const auto& line : debugData->lines)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = line;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }

        for (const auto& plane : debugData->planes)
        {
            pushConstants.viewProjection = frameData.camViewMatrix * frameData.projectionMatrix;
            pushConstants.model = plane;

            cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(WireframePushConstants), &pushConstants);
            cmd->drawIndexed(debugMeshAccessor.indexCount, 1, debugMeshAccessor.firstIndex, debugMeshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        }
        #endif
    }
}
#endif