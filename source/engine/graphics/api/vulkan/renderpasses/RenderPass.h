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
                   std::span<const AttachmentSlot> attachmentSlots,
                   std::span<const Subpass> subpasses,
                   std::vector<Attachment> attachments,
                   const AttachmentSize &size,
                   ClearValueFlags_t clearFlags);

        void Begin(vk::CommandBuffer *cmd, uint32_t attachmentIndex = 0u);
        void Execute(vk::CommandBuffer *cmd, const PerFrameRenderState &frameData, uint32_t frameIndex) const;
        void End(vk::CommandBuffer *cmd);

        auto GetAttachmentView(uint32_t index) const -> vk::ImageView;
        auto GetVkPass() const -> vk::RenderPass;

        void CreateFrameBuffers(std::span<const vk::ImageView>, Vector2 dimensions);

        template <std::derived_from<ITechnique> T>
        void RegisterTechnique(const Device& device, ShaderBindingManager *shaderBindingManager);
        void RegisterShadowMappingTechnique(vk::Device device, ShaderBindingManager *shaderBindingManager, uint32_t shadowCasterIndex, bool isOmniDirectional);

        template <std::derived_from<ITechnique> T>
        void UnregisterTechnique();
        void UnregisterShadowMappingTechnique();

    private:
        vk::Device m_device;
        vk::UniqueRenderPass m_renderPass;
        AttachmentSize m_attachmentSize;
        ClearValueFlags_t m_clearFlags;
        std::vector<std::unique_ptr<ITechnique>> m_litTechniques;
        std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
        std::vector<Attachment> m_attachments;
        std::vector<vk::UniqueFramebuffer> m_frameBuffers;
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
