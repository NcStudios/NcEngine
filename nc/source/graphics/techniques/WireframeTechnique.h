#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ecs/component/Component.h"
#include "ecs/component/DebugWidget.h"
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
        // MVP matrices
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

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            std::string m_meshPath;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}
#endif