#pragma once

#include "directx/Inc/DirectXMath.h"
#include "ecs/component/Component.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>

namespace nc::graphics
{
    class Graphics; class Base; class Swapchain;

    class PhongAndUiTechnique : public ITechnique
    {
        public:
            PhongAndUiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~PhongAndUiTechnique() noexcept;
            
            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;
            
            void Clear() noexcept;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}