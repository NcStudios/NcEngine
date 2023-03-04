#ifdef NC_EDITOR_ENABLED
#pragma once

#include "ecs/Component.h"
#include "graphics/DebugWidget.h"
#include "DirectXMath.h"
#include "ITechnique.h"

#ifdef NC_DEBUG_RENDERING_ENABLED
#include "graphics/debug/DebugRenderer.h"
#endif

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
    struct DebugData;
    class Device;
    class ShaderDescriptorSets;

    struct WireframePushConstants
    {
        // MVP matrices
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX viewProjection;
    };

    class WireframeTechnique : public ITechnique
    {
        public:
            WireframeTechnique(const Device& device, ShaderDescriptorSets*, vk::RenderPass* renderPass);
            ~WireframeTechnique() noexcept;

            bool CanBind(const PerFrameRenderState& frameData) override;
            void Bind(vk::CommandBuffer* cmd) override;

            bool CanRecord(const PerFrameRenderState& frameData) override;
            void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        private:
            std::string m_meshPath;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
            #ifdef NC_DEBUG_RENDERING_ENABLED
            graphics::DebugRenderer m_debugRenderer;
            #endif
    };
}
#endif