#pragma once

#include "Attachment.h"
#include "graphics/api/vulkan/pipelines/IPipeline.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingPipeline.h"
#include "graphics/shader_resource/RenderPassSinkBufferHandle.h"

#include <span>
#include <vector>

namespace nc::graphics
{
struct PerFrameRenderState;
struct PerFrameInstanceData;

namespace vulkan
{
class Device;
class ShaderBindingManager;

struct Pipeline
{
    size_t uid;
    std::unique_ptr<IPipeline> pipeline;
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
                   RenderPassSinkType sinkViewsType,
                   std::vector<vk::ImageView> sinkViews,
                   uint32_t sourceSinkPartition);

        void Begin(vk::CommandBuffer* cmd, uint32_t attachmentIndex = 0u);
        void Execute(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData& instanceData, uint32_t frameIndex) const;
        void End(vk::CommandBuffer* cmd);

        auto GetVkPass() const -> vk::RenderPass;
        auto GetAttachmentView(uint32_t index) const -> vk::ImageView { return m_attachments.at(index).view.get(); }

        void CreateFrameBuffer(std::span<const vk::ImageView>, Vector2 dimensions);
        template <std::derived_from<IPipeline> T>
        void RegisterPipeline(const Device* device, ShaderBindingManager* shaderBindingManager);
        
        template <std::derived_from<IPipeline> T>
        void UnregisterPipeline();
        void UnregisterPipelines();

        auto GetSinkViewsType() const noexcept -> RenderPassSinkType { return m_sinkViewsType; }
        auto GetSinkViews() const -> std::span<const vk::ImageView>;

    private:
        vk::Device m_device;
        vk::UniqueRenderPass m_renderPass;
        AttachmentSize m_attachmentSize;
        ClearValueFlags_t m_clearFlags;
        std::vector<Pipeline> m_pipelines;
        std::vector<Attachment> m_attachments;
        std::vector<vk::UniqueFramebuffer> m_frameBuffers;
        RenderPassSinkType m_sinkViewsType;
        std::vector<vk::ImageView> m_sinkViews;
        uint32_t m_sourceSinkPartition;
};

template <std::derived_from<IPipeline> T>
void RenderPass::UnregisterPipeline()
{
    const auto& techniqueType = typeid(T);
    const auto uid = techniqueType.hash_code();

    auto pos = std::ranges::find_if(m_pipelines, [uid](auto& pipeline)
    {
        return pipeline.uid == uid;
    });

    if (pos != m_pipelines.end())
    {
        pos->isActive = false;
    }
}

template <std::derived_from<IPipeline> T>
void RenderPass::RegisterPipeline(const Device* device, ShaderBindingManager* shaderBindingManager)
{
    const auto& techniqueType = typeid(T);
    auto uid = techniqueType.hash_code();

    auto pos = std::ranges::find_if(m_pipelines, [uid](Pipeline& pipeline)
    {
        return pipeline.uid == uid;
    });

    if (pos == m_pipelines.end())
    {
        auto pipeline = Pipeline
        {
            uid,
            std::make_unique<T>(*device, shaderBindingManager, m_renderPass.get()),
            true
        };
        m_pipelines.push_back(std::move(pipeline));
        return;
    }

    pos->isActive = true;
}
} // namespace nc::graphics
} // namespace vulkan
