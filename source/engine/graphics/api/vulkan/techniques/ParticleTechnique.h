#pragma once

#include "ITechnique.h"
#include "ecs/Component.h"
#include "particle/EmitterState.h"

#include "DirectXMath.h"
#include "vulkan/vk_mem_alloc.hpp"

#include <vector>

namespace nc::graphics
{
    class Device;
    class ShaderDescriptorSets;

    struct ParticlePushConstants
    {
        // N MVP matrices
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX viewProjection;

        // Indices into texture array
        uint32_t baseColorIndex;
    };

    class ParticleTechnique : public ITechnique
    {
    public:
        ParticleTechnique(const Device& device, ShaderDescriptorSets* descriptorSets, vk::RenderPass* renderPass);
        ~ParticleTechnique() noexcept;

        bool CanBind(const PerFrameRenderState& frameData) override;
        void Bind(vk::CommandBuffer* cmd) override;

        bool CanRecord(const PerFrameRenderState& frameData) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        void Clear() noexcept;
        bool IsShadowMapTechnique() override {return false;} //@todo: total HACK.

    private:
        ShaderDescriptorSets* m_descriptorSets;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
    };
}
