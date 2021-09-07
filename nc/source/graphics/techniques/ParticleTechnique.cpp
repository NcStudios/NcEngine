// #include "ParticleTechnique.h"
// #include "Ecs.h"
// #include "config/Config.h"
// #include "component/Transform.h"
// #include "debug/Profiler.h"
// #include "graphics/Graphics.h"
// #include "graphics/Commands.h"
// #include "graphics/Initializers.h"
// #include "graphics/ShaderUtilities.h"
// #include "graphics/MeshManager.h"
// #include "graphics/Swapchain.h"
// #include "graphics/Base.h"
// #include "graphics/resources/ImmutableBuffer.h"
// #include "graphics/resources/ResourceManager.h"

// namespace nc::graphics
// {
//     ParticleTechnique::ParticleTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass)
//     : m_emitterStates{},
//       m_graphics{graphics},
//       m_base{graphics->GetBasePtr()},
//       m_swapchain{graphics->GetSwapchainPtr()},
//       m_pipeline{},
//       m_pipelineLayout{}
//     {
//         CreatePipeline(renderPass);
//     }

//     ParticleTechnique::~ParticleTechnique()
//     {
//         auto device = m_base->GetDevice();
//         device.destroyPipelineLayout(m_pipelineLayout);
//         device.destroyPipeline(m_pipeline);
//     }

//     void ParticleTechnique::CreatePipeline(vk::RenderPass* renderPass)
//     {
//         // Shaders
//         auto defaultShaderPath = nc::config::GetGraphicsSettings().shadersPath;
//         auto vertexShaderByteCode = ReadShader(defaultShaderPath + "ParticleVertex.spv");
//         auto fragmentShaderByteCode = ReadShader(defaultShaderPath + "ParticleFragment.spv");

//         auto vertexShaderModule = CreateShaderModule(vertexShaderByteCode, m_base);
//         auto fragmentShaderModule = CreateShaderModule(fragmentShaderByteCode, m_base);

//         vk::PipelineShaderStageCreateInfo shaderStages[] = 
//         {
//             CreatePipelineShaderStageCreateInfo(ShaderStage::Vertex, vertexShaderModule),
//             CreatePipelineShaderStageCreateInfo(ShaderStage::Pixel, fragmentShaderModule)
//         };

//         auto pushConstantRange = CreatePushConstantRange(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, sizeof(ParticlePushConstants)); // PushConstants
//         std::vector<vk::DescriptorSetLayout> descriptorLayouts = {*ResourceManager::GetTexturesDescriptorSetLayout(), *ResourceManager::GetPointLightsDescriptorSetLayout()};
//         auto pipelineLayoutInfo = CreatePipelineLayoutCreateInfo(pushConstantRange, descriptorLayouts);
//         m_pipelineLayout = m_base->GetDevice().createPipelineLayout(pipelineLayoutInfo);

//         std::array<vk::DynamicState, 2> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
//         vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
//         dynamicStateInfo.setDynamicStateCount(dynamicStates.size());
//         dynamicStateInfo.setDynamicStates(dynamicStates);

//         // Graphics pipeline
//         vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
//         pipelineCreateInfo.setStageCount(2); // Shader stages
//         pipelineCreateInfo.setPStages(shaderStages); // Shader stages
//         auto vertexBindingDescription = GetVertexBindingDescription();
//         auto vertexAttributeDescription = GetVertexAttributeDescriptions();
//         auto vertexInputInfo = CreateVertexInputCreateInfo(vertexBindingDescription, vertexAttributeDescription);
//         pipelineCreateInfo.setPVertexInputState(&vertexInputInfo);
//         auto inputAssembly = CreateInputAssemblyCreateInfo();
//         pipelineCreateInfo.setPInputAssemblyState(&inputAssembly);
//         auto viewportState = CreateViewportCreateInfo();
//         pipelineCreateInfo.setPViewportState(&viewportState);
//         auto rasterizer = CreateRasterizationCreateInfo(vk::PolygonMode::eFill, 1.0f);
//         pipelineCreateInfo.setPRasterizationState(&rasterizer);
//         auto multisampling = CreateMulitsampleCreateInfo();
//         pipelineCreateInfo.setPMultisampleState(&multisampling);
//         auto depthStencil = CreateDepthStencilCreateInfo();
//         pipelineCreateInfo.setPDepthStencilState(&depthStencil);
//         auto colorBlendAttachment = CreateColorBlendAttachmentCreateInfo(true);
//         auto colorBlending = CreateColorBlendStateCreateInfo(colorBlendAttachment, true);
//         pipelineCreateInfo.setPColorBlendState(&colorBlending);
//         pipelineCreateInfo.setPDynamicState(&dynamicStateInfo);
//         pipelineCreateInfo.setLayout(m_pipelineLayout);
//         pipelineCreateInfo.setRenderPass(*renderPass); // Can eventually swap out and combine render passes but they have to be compatible. see: https://www.khronos.org/registry/specs/1.0/html/vkspec.html#renderpass-compatibility
//         pipelineCreateInfo.setSubpass(0); // The index of the subpass where this graphics pipeline where be used.
//         pipelineCreateInfo.setBasePipelineHandle(nullptr); // Graphics pipelines can be created by deriving from existing, similar pipelines. 
//         pipelineCreateInfo.setBasePipelineIndex(-1); // Similarly, switching between pipelines from the same parent can be done.

//         m_pipeline = m_base->GetDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
//         m_base->GetDevice().destroyShaderModule(vertexShaderModule, nullptr);
//         m_base->GetDevice().destroyShaderModule(fragmentShaderModule, nullptr);
//     }

//     void ParticleTechnique::Bind(vk::CommandBuffer* cmd)
//     {
//         NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
//         cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
//         cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, 1, ResourceManager::GetTexturesDescriptorSet(), 0, 0);
//         NC_PROFILE_END();
//     }

//     void ParticleTechnique::RegisterEmitters(std::vector<particle::EmitterState>* emitterStates)
//     {
//         m_emitterStates = emitterStates;
//     }

//     void DrawIndexed(vk::CommandBuffer* cmd, const nc::graphics::Mesh& meshAccessor)
//     {
//         NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
//         cmd->drawIndexed(meshAccessor.indicesCount, 1, meshAccessor.firstIndex, meshAccessor.firstVertex, 0); // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
//         NC_PROFILE_END();
//     }

//     void ParticleTechnique::Record(vk::CommandBuffer* cmd)
//     {
//         NC_PROFILE_BEGIN(debug::profiler::Filter::Rendering);
//         const auto& viewMatrix = m_graphics->GetViewMatrix();
//         const auto& projectionMatrix = m_graphics->GetProjectionMatrix();

//         auto pushConstants = ParticlePushConstants{};
//         pushConstants.viewProjection = viewMatrix * projectionMatrix;
//         pushConstants.cameraPos = m_graphics->GetCameraPosition();

//         const auto meshAccessor = ResourceManager::GetMeshAccessor("project/assets/mesh/plane.nca");
        
//         for (auto& emitterState : *m_emitterStates)
//         {
//             auto [index, matrices] = emitterState.GetSoA()->View<particle::EmitterState::MvpMatricesIndex>();

//             for(; index.Valid(); ++index)
//             {
//                 pushConstants.model = matrices[index].modelView; // Is actually untransposed model matrix, see MvpMatrices.h
//                 pushConstants.baseColorIndex =  ResourceManager::GetTextureAccessor(emitterState.GetInfo()->init.particleTexturePath);

//                 cmd->pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex, 0, sizeof(ParticlePushConstants), &pushConstants);
//                 DrawIndexed(cmd, meshAccessor);
//             }
            
//             #ifdef NC_EDITOR_ENABLED
//             m_graphics->IncrementDrawCallCount();
//             #endif
//         }
//         NC_PROFILE_END();
//     }

//     void ParticleTechnique::Clear()
//     {
//         // @todo: This whole storage strategy will be changed, this is really just a stand-in until then.
//         m_emitterStates = nullptr;
//     }
// }