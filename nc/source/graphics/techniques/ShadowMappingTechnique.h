#pragma once

#include "ECS.h"

#include "directx/math/DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics; class Commands; class Base; class Swapchain;

    struct ShadowMappingPushConstants
    {
        // Light VP matrix
        DirectX::XMMATRIX lightViewProjection;
    };

    class ShadowMappingTechnique
    {
        public:
            ShadowMappingTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~ShadowMappingTechnique();

            void Bind(vk::CommandBuffer* cmd);
            void Record(vk::CommandBuffer* cmd, std::span<nc::PointLight> pointLights, std::span<nc::MeshRenderer> meshRenderers);

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
    };
}