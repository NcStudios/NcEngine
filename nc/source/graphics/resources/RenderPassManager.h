#pragma once

#include "graphics/Initializers.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <string>

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
            RenderPassManager(Graphics* graphics, const Vector2& dimensions);
            ~RenderPassManager();

            RenderPass& Acquire(const std::string& uid);
            void Create(const std::string& uid, std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses, ClearValue valuesToClear, const Vector2& dimensions);
            void Resize(const Vector2& dimensions, vk::Extent2D extent);

            void Begin(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex);
            void End(vk::CommandBuffer* cmd);

            void RegisterAttachments(std::vector<vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index);
            void RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid);
            
        private:
            RenderTarget& GetRenderTarget(const std::string& uid, uint32_t index);

            Graphics* m_graphics;
            std::vector<RenderPass> m_renderPasses;
            std::vector<RenderTarget> m_renderTargets;
    };

} // namespace nc