#pragma once

#include "graphics/renderpasses/Attachment.h"

namespace nc::graphics
{
class GpuOptions;
class ShaderDescriptorSets;

class RenderPass
{
    public:
        RenderPass(vk::Device device,
                   uint8_t priority,
                   std::string uid,
                   std::vector<AttachmentSlot> attachmentSlots,
                   std::vector<Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags);

        void Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex);
        void Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData) const;
        void End(vk::CommandBuffer *cmd);

        auto GetPriority() const -> uint32_t;
        auto GetAttachmentView(uint32_t index) const -> vk::ImageView;
        auto GetUid() const -> std::string;
        auto GetVkPass() const -> vk::RenderPass;

        void RegisterAttachmentViews(std::vector<vk::ImageView>, Vector2 dimensions, uint32_t index);

        template <std::derived_from<ITechnique> T>
        void RegisterTechnique(vk::Device device, GpuOptions *gpuOptions, ShaderDescriptorSets *descriptorSets);
        void RegisterShadowMappingTechnique(vk::Device device, GpuOptions *gpuOptions, ShaderDescriptorSets *descriptorSets, uint32_t shadowCasterIndex);

        template <std::derived_from<ITechnique> T>
        void UnregisterTechnique();

    private:
        auto GetFrameBuffer(uint32_t index) -> vk::Framebuffer;
        vk::Device m_device;
        uint8_t m_priority;
        std::string m_uid;
        vk::UniqueRenderPass m_renderPass;
        AttachmentSize m_attachmentSize;
        ClearValueFlags_t m_clearFlags;
        std::vector<std::unique_ptr<ITechnique>> m_techniques;
        std::vector<Attachment> m_attachments;
        std::vector<FrameBuffer> m_frameBuffers;
};

template <std::derived_from<ITechnique> T>
void RenderPass::RegisterTechnique(vk::Device device, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets)
{
    UnregisterTechnique<T>();
    m_techniques.push_back(std::make_unique<T>(device, gpuOptions, descriptorSets, &m_renderPass.get()));
}

template <std::derived_from<ITechnique> T>
void RenderPass::UnregisterTechnique()
{
    const auto &techniqueType = typeid(T);
    auto techniquePos = std::ranges::find_if(m_techniques, [&techniqueType](const auto &foundTechnique)
                                             { return (typeid(foundTechnique) == techniqueType); });

    if (techniquePos != m_techniques.end())
    {
        *techniquePos = std::move(m_techniques.back());
        m_techniques.pop_back();
    }
}
}