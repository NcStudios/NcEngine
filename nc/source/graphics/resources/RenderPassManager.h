#pragma once

#include "graphics/Initializers.h"
#include "graphics/techniques/ITechnique.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <concepts>
#include <string>
#include <typeinfo>

namespace nc::graphics
{
    class Graphics;

    struct RenderTarget
    {
        std::string renderPassUid;
        uint32_t index;
        std::vector<vk::ImageView> attachmentHandles;
        vk::UniqueFramebuffer frameBuffer;
    };

    class RenderPassManager
    {
        public:
            inline static const std::string LitShadingPass = "Lit Pass";
            inline static const std::string ShadowMappingPass = "Shadow Mapping Pass";
            
            RenderPassManager(Graphics* graphics, const Vector2& dimensions);
            ~RenderPassManager() noexcept;

            void Execute(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex, const PerFrameRenderState& frameData);
            RenderPass& Acquire(const std::string& uid);

            void RegisterAttachments(std::vector<vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index);
            void RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid);

            template <std::derived_from<ITechnique> T>
            void RegisterTechnique(const std::string& uid);
            
        private:
            void Create(const std::string& uid, std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses, ClearValue valuesToClear, const Vector2& dimensions);
            void Resize(const Vector2& dimensions, vk::Extent2D extent);
            void Begin(RenderPass* renderPass, vk::CommandBuffer* cmd, uint32_t renderTargetIndex);
            void End(vk::CommandBuffer* cmd);
            RenderTarget& GetRenderTarget(const std::string& uid, uint32_t index);

            Graphics* m_graphics;
            std::vector<RenderPass> m_renderPasses;
            std::vector<RenderTarget> m_renderTargets;
    };

    template <std::derived_from<ITechnique> T>
    void RenderPassManager::RegisterTechnique(const std::string& uid)
    {
        const auto& techniqueType = typeid(T);
        auto& renderpass = Acquire(uid);
        auto techniquePos = std::ranges::find_if(renderpass.techniques, [&techniqueType](const auto& foundTechnique)
        {
            return (typeid(foundTechnique) == techniqueType);
        });

        if (techniquePos != renderpass.techniques.end())
        {
            *techniquePos = std::move(renderpass.techniques.back());
            renderpass.techniques.pop_back();
        }

        renderpass.techniques.push_back(std::make_unique<T>(m_graphics, &renderpass.renderpass.get()));
    }

} // namespace nc