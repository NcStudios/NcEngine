#ifdef NC_EDITOR_ENABLED

#pragma once

#include "component/Component.h"
#include "vulkan/vk_mem_alloc.hpp"
#include "directx/math/DirectXMath.h"
#include "component/DebugWidget.h"

#include <vector>
#include <span>
#include <optional>

namespace nc
{
    class MeshRenderer; 
}

namespace nc::graphics
{
    class Graphics2; class Commands; class Base; class Swapchain;

    struct WireframePushConstants
    {
        // N MVP matrices
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX viewProjection;
    };

    class WireframeTechnique
    {
        public:
            WireframeTechnique(nc::graphics::Graphics2* graphics, vk::RenderPass* renderPass);
            ~WireframeTechnique();

            void Bind(vk::CommandBuffer* cmd);
            void Record(vk::CommandBuffer* cmd);

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::DebugWidget debugWidget);
            void ClearDebugWidget();
            bool HasDebugWidget() const;
            #endif

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            #ifdef NC_EDITOR_ENABLED
            std::optional<nc::DebugWidget> m_debugWidget;
            #endif

            nc::graphics::Graphics2* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
    };
}
#endif