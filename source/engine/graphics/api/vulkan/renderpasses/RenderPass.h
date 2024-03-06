#pragma once

#include "Attachment.h"
#include "graphics/api/vulkan/techniques/ITechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"

#include <span>
#include <vector>

namespace nc::graphics
{
struct PerFrameRenderState;

namespace vulkan
{
class Device;
class ShaderBindingManager;

class RenderPass
{
    public:
        RenderPass(vk::Device device,
                   std::string uid,
                   std::span<const AttachmentSlot> attachmentSlots,
                   std::span<const Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags);

        void Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex);
        void Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData, uint32_t frameIndex) const;
        void End(vk::CommandBuffer *cmd);

        auto GetAttachmentView(uint32_t index) const -> vk::ImageView;
        auto GetUid() const -> std::string;
        auto GetVkPass() const -> vk::RenderPass;

        void RegisterAttachmentViews(std::span<const vk::ImageView>, Vector2 dimensions, uint32_t index);

        template <std::derived_from<ITechnique> T>
        void RegisterTechnique(const Device& device, ShaderBindingManager *shaderBindingManager);
        void RegisterShadowMappingTechnique(vk::Device device, ShaderBindingManager *shaderBindingManager, uint32_t shadowCasterIndex);

        template <std::derived_from<ITechnique> T>
        void UnregisterTechnique();
        void UnregisterShadowMappingTechnique();

        void ClearTechniques() { m_litTechniques.clear(); m_shadowMappingTechniques.clear(); }

    private:
        auto GetFrameBuffer(uint32_t index) -> vk::Framebuffer;
        vk::Device m_device;
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
void RenderPass::RegisterTechnique(const Device& device, ShaderBindingManager* shaderBindingManager)
{
    UnregisterTechnique<T>();
    m_litTechniques.push_back(std::make_unique<T>(device, shaderBindingManager, &m_renderPass.get()));
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
} // namespace vulkan
