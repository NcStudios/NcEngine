#include "ParticleTechnique.h"
#include "assets/AssetService.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "graphics/api/vulkan/buffers/ImmutableBuffer.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/api/vulkan/shaders/ShaderUtilities.h"
#include "graphics/api/vulkan/shaders/ShaderDescriptorSets.h"
#include "graphics/api/vulkan/shaders/ShaderResources.h"
#include "graphics/api/vulkan/meshes/VertexDescriptions.h"
#include "graphics/PerFrameRenderState.h"

#include "optick.h"

namespace nc::graphics
{
    ParticleTechnique::ParticleTechnique(const Device& device, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass)
        : m_descriptorSets{descriptorSets},
          m_pipeline{nullptr},
          m_pipelineLayout{nullptr}
    {
        const auto vkDevice = device.VkDevice();

        // Shaders
        auto defaultShaderPath = nc::config::GetAssetSettings().shadersPath;
        auto vertexShaderByteCode = ReadShader(defaultShaderPath + "ParticleVertex.spv");
        auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "ParticleFragment.spv");

        auto vertexShaderModule = CreateShaderModule(vkDevice, vertexShaderByteCode);
        auto fragmentShaderModule = CreateShaderModule(vkDevice, fragmentShaderByteCode);

        std::array<vk::PipelineShaderStageCreateInfo, 2u> shaderStages
        {
            CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
            CreatePipelineShaderStageCreateInfo(ShaderStage::Fragment, fragmentShaderModule)
        };

        auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, sizeof(ParticlePushConstants)); // PushConstants

        std::array<vk::DescriptorSetLayout, 1u> descriptorLayouts
        {
            *(m_descriptorSets->GetSetLayout(BindFrequency::per_frame))
        };

        auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
        m_pipelineLayout = vkDevice.createPipelineLayoutUnique(pipelineLayoutInfo);

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
        auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill, 1.0f);
        pipelineCreateInfo.setPRasterizationState(&rasterizer);
        auto multisampling = CreateMultisampleCreateInfo(device.GetGpuOptions().GetMaxSamplesCount());
        pipelineCreateInfo.setPMultisampleState(&multisampling);
        auto depthStencil = CreateDepthStencilCreateInfo();
        pipelineCreateInfo.setPDepthStencilState(&depthStencil);
        auto colorBlendAttachment = CreateColorBlendAttachmentCreateInfo(true);
        auto colorBlending = CreateColorBlendStateCreateInfo(colorBlendAttachment, true);
        pipelineCreateInfo.setPColorBlendState(&colorBlending);
        pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
        pipelineCreateInfo.setLayout(m_pipelineLayout.get());
        pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/specs/1.0/html/vkspec.html#renderpass-compatibility
        pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
        pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
        pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

        m_pipeline = vkDevice.createGraphicsPipelineUnique(nullptr, pipelineCreateInfo).value;

        vkDevice.destroyShaderModule(vertexShaderModule, nullptr);
        vkDevice.destroyShaderModule(fragmentShaderModule, nullptr);
    }

    ParticleTechnique::~ParticleTechnique() noexcept
    {
        m_pipeline.reset();
        m_pipelineLayout.reset();
    }

    bool ParticleTechnique::CanBind(const PerFrameRenderState& frameData)
    {
        return frameData.emitterStates.size() > 0;
    }

    void ParticleTechnique::Bind(vk::CommandBuffer* cmd)
    {
        OPTICK_CATEGORY("ParticleTechnique::Bind", Optick::Category::Rendering);
        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());
        m_descriptorSets->BindSet(BindFrequency::per_frame, cmd, vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0);
    }

    bool ParticleTechnique::CanRecord(const PerFrameRenderState& frameData)
    {
        return frameData.emitterStates.size() > 0;
    }

    void ParticleTechnique::Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("ParticleTechnique::Record", Optick::Category::Rendering);
        auto pushConstants = ParticlePushConstants{};
        pushConstants.viewProjection = frameData.cameraState.view * frameData.cameraState.projection;
        const auto meshAccessor = AssetService<MeshView>::Get()->Acquire(nc::asset::PlaneMesh);

        for (auto& emitterState : frameData.emitterStates)
        {
            auto [index, models] = emitterState.GetSoA()->View<particle::EmitterState::ModelMatrixIndex>();

            for (; index.Valid(); ++index)
            {
                pushConstants.model = models[index];
                pushConstants.baseColorIndex = AssetService<TextureView>::Get()->Acquire(emitterState.GetInfo().init.particleTexturePath).index;

                cmd->pushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, sizeof(ParticlePushConstants), &pushConstants);
                cmd->drawIndexed(meshAccessor.indexCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
            }
        }
    }
}
