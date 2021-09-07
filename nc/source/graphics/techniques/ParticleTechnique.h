// #pragma once

// #include "component/Component.h"
// #include "particle/EmitterState.h"

// #include "vulkan/vk_mem_alloc.hpp"
// #include <vector>
// #include "directx/math/DirectXMath.h"

// namespace nc::graphics
// {
//     class Graphics; class Commands; class Base; class Swapchain;

//     struct ParticlePushConstants
//     {
//         // N MVP matrices
//         DirectX::XMMATRIX model;
//         DirectX::XMMATRIX viewProjection;

//         // Camera world position
//         Vector3 cameraPos;

//         // Indices into texture array
//         uint32_t baseColorIndex;
//         uint32_t normalColorIndex;
//         uint32_t roughnessColorIndex;
//     };

//     class ParticleTechnique
//     {
//         public:
//             ParticleTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
//             ~ParticleTechnique();
            
//             void Bind(vk::CommandBuffer* cmd);
//             void RegisterEmitters(std::vector<particle::EmitterState>* emitterStates);
//             void Record(vk::CommandBuffer* cmd);
//             void Clear();

//         private:
//             void CreatePipeline(vk::RenderPass* renderPass);

//             std::vector<particle::EmitterState>* m_emitterStates;
//             nc::graphics::Graphics* m_graphics;
//             Base* m_base;
//             Swapchain* m_swapchain;
//             vk::Pipeline m_pipeline;
//             vk::PipelineLayout m_pipelineLayout;
//     };
// }