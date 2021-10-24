#ifdef NC_EDITOR_ENABLED

#pragma once

#include "component/Component.h"
#include "component/DebugWidget.h"
#include "directx/math/DirectXMath.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include <span>
#include <optional>

namespace nc
{
    class MeshRenderer; 
}

namespace nc::graphics
{
    class Graphics; class Base; class Swapchain;

    struct WireframePushConstants
    {
        // N MVP matrices
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX viewProjection;
    };

    class WireframeTechnique : public ITechnique
    {
        public:
            WireframeTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~WireframeTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            std::optional<nc::DebugWidget> m_debugWidget;
            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}
#endif