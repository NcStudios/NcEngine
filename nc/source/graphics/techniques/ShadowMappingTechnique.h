#pragma once

#include "ECS.h"

#include "directx/math/DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics; class Commands; class Base; class Swapchain;

    // Depth bias (and slope) are used to avoid shadowing artifacts
    // Constant depth bias factor (always applied)
    const float DEPTH_BIAS_CONSTANT = 1.25f;

    // Slope depth bias factor, applied depending on polygon's slope
    const float DEPTH_BIAS_SLOPE = 1.75f;

    // Angle of light's field of view.
    const float LIGHT_FIELD_OF_VIEW = 45.0f;

    struct ShadowMappingPushConstants
    {
        // Light MVP matrix
        DirectX::XMMATRIX depthMVP;
    };

    class ShadowMappingTechnique
    {
        public:
            ShadowMappingTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~ShadowMappingTechnique();

            void Bind(vk::CommandBuffer* cmd);
            void Record(vk::CommandBuffer* cmd, registry_type* registry, std::span<nc::PointLight> pointLights, std::span<nc::MeshRenderer> meshRenderers);

        private:
            void CreatePipeline(vk::RenderPass* renderPass);
            DirectX::XMMATRIX CalculateViewMatrix(Transform* lightTransform);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
            DirectX::XMMATRIX m_depthProjectionMatrix;
            DirectX::XMMATRIX m_modelMatrix;
    };
}