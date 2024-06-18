#pragma once

#include "Attachment.h"
#include "graphics/api/vulkan/techniques/ITechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"
#include "graphics/shader_resource/PPImageArrayBufferHandle.h"

#include <span>
#include <vector>

namespace nc::graphics
{
struct PerFrameRenderState;

namespace vulkan
{
class Device;
class ShaderBindingManager;

struct Pipeline
{
    size_t uid;
    std::unique_ptr<ITechnique> pipeline;
    bool isActive;
};

class RenderPass
{
    public:
        RenderPass(vk::Device device,
                   std::span<const AttachmentSlot> attachmentSlots,
                   std::span<const Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags);

        RenderPass(vk::Device device,
                   std::span<const AttachmentSlot> attachmentSlots,
                   std::span<const Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags,
                   PostProcessImageType renderTargetsType,
                   std::vector<vk::ImageView> renderTargets,
                   uint32_t sourceSinkPartition);

        void Begin(vk::CommandBuffer* cmd, uint32_t attachmentIndex = 0u);
        void Execute(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, uint32_t frameIndex) const;
        void End(vk::CommandBuffer* cmd);

        auto GetVkPass() const -> vk::RenderPass;

        void CreateFrameBuffer(std::span<const vk::ImageView>, Vector2 dimensions);

        template <std::derived_from<ITechnique> T>
        void RegisterPipeline(const Device* device, ShaderBindingManager* shaderBindingManager);
        void RegisterShadowMappingTechnique(vk::Device device, ShaderBindingManager* shaderBindingManager, uint32_t shadowCasterIndex, bool isOmniDirectional);
        
        template <std::derived_from<ITechnique> T>
        void UnregisterPipeline();
        void UnregisterShadowMappingTechnique();

        void UnregisterPipelines();

        auto GetRenderTargetsType() const noexcept -> PostProcessImageType { return m_renderTargetsType; }
        auto GetRenderTargets() const -> std::span<const vk::ImageView>;

    private:
        vk::Device m_device;
        vk::UniqueRenderPass m_renderPass;
        AttachmentSize m_attachmentSize;
        ClearValueFlags_t m_clearFlags;
        std::vector<Pipeline> m_litPipelines;
        std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
        std::vector<Attachment> m_attachments;
        std::vector<vk::UniqueFramebuffer> m_frameBuffers;
        PostProcessImageType m_renderTargetsType;
        std::vector<vk::ImageView> m_renderTargets;
        uint32_t m_sourceSinkPartition;
};

template <std::derived_from<ITechnique> T>
void RenderPass::UnregisterPipeline()
{
    const auto& techniqueType = typeid(T);
    const auto uid = techniqueType.hash_code();

    auto pos = std::ranges::find_if(m_litPipelines, [uid](auto& pipeline)
    {
        return pipeline.uid == uid;
    });

    if (pos != m_litPipelines.end())
    {
        pos->isActive = false;
    }
}

template <std::derived_from<ITechnique> T>
void RenderPass::RegisterPipeline(const Device* device, ShaderBindingManager* shaderBindingManager)
{
    const auto& techniqueType = typeid(T);
    auto uid = techniqueType.hash_code();

    auto pos = std::ranges::find_if(m_litPipelines, [uid](Pipeline& pipeline)
    {
        return pipeline.uid == uid;
    });

    if (pos == m_litPipelines.end())
    {
        auto pipeline = Pipeline
        {
            uid,
            std::make_unique<T>(*device, shaderBindingManager, &m_renderPass.get()),
            true
        };
        m_litPipelines.push_back(std::move(pipeline));
        return;
    }

    pos->isActive = true;
}
} // namespace nc::graphics
} // namespace vulkan
