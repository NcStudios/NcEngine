#pragma once

#include "Attachment.h"
#include "graphics/api/vulkan/techniques/ITechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"

#include <span>
#include <vector>

namespace nc::graphics
{
class Device;
struct PerFrameRenderState;
class ShaderDescriptorSets;

class RenderPass
{
    public:
        RenderPass(vk::Device device,
                   uint8_t priority,
                   std::string uid,
                   std::span<const AttachmentSlot> attachmentSlots,
                   std::span<const Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags);

        void Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex);
        void Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData, uint32_t frameIndex) const;
        void End(vk::CommandBuffer *cmd);

        auto GetPriority() const -> uint32_t;
        auto GetAttachmentView(uint32_t index) const -> vk::ImageView;
        auto GetUid() const -> std::string;
        auto GetVkPass() const -> vk::RenderPass;

        void RegisterAttachmentViews(std::span<const vk::ImageView>, Vector2 dimensions, uint32_t index);

        template <std::derived_from<ITechnique> T>
        void RegisterTechnique(const Device& device, ShaderDescriptorSets *descriptorSets);
        void RegisterShadowMappingTechnique(vk::Device device, ShaderDescriptorSets *descriptorSets, uint32_t shadowCasterIndex);

        template <std::derived_from<ITechnique> T>
        void UnregisterTechnique();
        void UnregisterShadowMappingTechnique();

    private:
        auto GetFrameBuffer(uint32_t index) -> vk::Framebuffer;
        vk::Device m_device;
        uint8_t m_priority;
        std::string m_uid;
        vk::UniqueRenderPass m_renderPass;
        AttachmentSize m_attachmentSize;
        ClearValueFlags_t m_clearFlags;
        std::vector<std::unique_ptr<ITechnique>> m_litTechniques;
        std::vector<std::unique_ptr<ShadowMappingTechnique>> m_shadowMappingTechniques;
        std::vector<Attachment> m_attachments;
        std::vector<FrameBuffer> m_frameBuffers;
};

template <std::derived_from<ITechnique> T>
void RenderPass::RegisterTechnique(const Device& device, ShaderDescriptorSets* descriptorSets)
{
    UnregisterTechnique<T>();
    m_litTechniques.push_back(std::make_unique<T>(device, descriptorSets, &m_renderPass.get()));
}

template <std::derived_from<ITechnique> T>
void RenderPass::UnregisterTechnique()
{
    const auto &techniqueType = typeid(T);
    auto techniquePos = std::ranges::find_if(m_litTechniques, [&techniqueType](const auto &foundTechnique)
                                             { return (typeid(foundTechnique) == techniqueType); });

    if (techniquePos != m_litTechniques.end())
    {
        *techniquePos = std::move(m_litTechniques.back());
        m_litTechniques.pop_back();
    }
}
} // namespace nc::graphics
