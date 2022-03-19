#pragma once

#include "ecs/component/Component.h"
#include "directx/Inc/DirectXMath.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>
#include <optional>

namespace nc::graphics
{
    class Graphics; class Base; class Swapchain;

    class UiTechnique : public ITechnique
    {
	    public:
	        UiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
	        ~UiTechnique() noexcept;

	        bool CanBind(const PerFrameRenderState& frameData) override;
	        void Bind(vk::CommandBuffer* cmd) override;

	        bool CanRecord(const PerFrameRenderState& frameData) override;
	        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

	    private:
	        void CreatePipeline(vk::RenderPass* renderPass);

	        nc::graphics::Graphics* m_graphics;
	        Base* m_base;
	        vk::UniquePipeline m_pipeline;
	        vk::UniquePipelineLayout m_pipelineLayout;
    };
}