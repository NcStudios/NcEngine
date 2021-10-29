#pragma once

#include "directx/math/DirectXMath.h"
#include "ecs/component/Component.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>

namespace nc::graphics
{
    class Graphics; class Base; class Swapchain;

    struct PhongPushConstants
    {
        // Camera world position
        Vector3 cameraPos;
        float padding;
    };

    class PhongAndUiTechnique : public ITechnique
    {
        public:
            PhongAndUiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~PhongAndUiTechnique() noexcept;
            
            bool CanBind(PerFrameRenderState* frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(PerFrameRenderState* frameData) override;
            void Record(vk::CommandBuffer* cmd, PerFrameRenderState* frameData) override;
            
            void Clear() noexcept;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}