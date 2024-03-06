#pragma once

#include "ITechnique.h"
#include "ecs/Component.h"
#include "particle/EmitterState.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

#include <vector>

namespace nc::graphics::vulkan
{
    class Device;
    class ShaderBindingManager;

    struct ParticlePushConstants
    {
        // N MVP matrices
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX viewProjection;

        // Indices into texture array
        uint32_t baseColorIndex;
        uint32_t normalColorIndex;
        uint32_t roughnessColorIndex;
    };

    class ParticleTechnique : public ITechnique
    {
    public:
        ParticleTechnique(const Device& device, ShaderBindingManager* shaderBindingManager, vk::RenderPass* renderPass);
        ~ParticleTechnique() noexcept;

        bool CanBind(const PerFrameRenderState& frameData) override;
        void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) override;

        bool CanRecord(const PerFrameRenderState& frameData) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        void Clear() noexcept;

    private:
        ShaderBindingManager* m_shaderBindingManager;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
    };
} // namespace nc::graphics::vulkan
