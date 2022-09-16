#pragma once

#include "DirectXMath.h"
#include "ecs/Component.h"
#include "graphics/resources/ShaderDescriptorSets.h"
#include "particle/EmitterState.h"
#include "ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>

namespace nc::graphics
{
    class Graphics; class Commands; class Base; class Swapchain;

    const auto PlaneMeshPath = std::string{ "plane.nca" };

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
        ParticleTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
        ~ParticleTechnique() noexcept;

        bool CanBind(const PerFrameRenderState& frameData) override;
        void Bind(vk::CommandBuffer* cmd) override;

        bool CanRecord(const PerFrameRenderState& frameData) override;
        void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) override;

        void Clear() noexcept;

    private:
        void CreatePipeline(vk::RenderPass* renderPass);

        Graphics* m_graphics;
        Base* m_base;
        Swapchain* m_swapchain;
        ShaderDescriptorSets* m_descriptorSets;
        vk::UniquePipeline m_pipeline;
        vk::UniquePipelineLayout m_pipelineLayout;
    };
}