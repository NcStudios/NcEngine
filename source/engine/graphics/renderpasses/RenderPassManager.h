#pragma once

#include "graphics/renderpasses/RenderPass.h"

#include <concepts>
#include <string>
#include <typeinfo>

namespace nc::graphics
{
    class GpuOptions;
    class ShaderDescriptorSets;
    class Swapchain;

    struct FrameBufferAttachment
    {
        std::string renderPassUid;
        uint32_t index;
        std::span<const vk::ImageView> attachmentHandles;
        vk::UniqueFramebuffer frameBuffer;
    };

    class RenderPassManager
    {
        public:
            RenderPassManager(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets, const Vector2& dimensions);
            ~RenderPassManager() noexcept;

            void Add(const std::string& uid, std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses, ClearValueFlags_t clearFlags, const Vector2& dimensions);
            void Remove(const std::string& uid);
            void Execute(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex, const PerFrameRenderState& frameData);
            RenderPass& Acquire(const std::string& uid);

            void RegisterAttachments(std::span<const vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index);
            void RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid);
            
            template <std::derived_from<ITechnique> T>
            void RegisterTechnique(const std::string& uid);
            
            template <std::derived_from<ITechnique> T>
            void UnregisterTechnique(const std::string& uid);

        private:
            void Resize(const Vector2& dimensions, vk::Extent2D extent);
            void Begin(RenderPass* renderPass, vk::CommandBuffer* cmd, uint32_t renderTargetIndex);
            void End(vk::CommandBuffer* cmd);
            FrameBufferAttachment& GetFrameBufferAttachment(const std::string& uid, uint32_t index);

            vk::Device m_device;
            Swapchain* m_swapchain;
            GpuOptions* m_gpuOptions;
            ShaderDescriptorSets* m_descriptorSets;
            std::vector<RenderPass> m_renderPasses;
            std::vector<FrameBufferAttachment> m_frameBufferAttachments;
    };

    template <std::derived_from<ITechnique> T>
    void RenderPassManager::RegisterTechnique(const std::string& uid)
    {
        UnregisterTechnique<T>(uid);
        auto& renderpass = Acquire(uid);
        renderpass.techniques.push_back(std::make_unique<T>(m_device, m_gpuOptions, m_descriptorSets, &renderpass.renderpass.get()));
    }

    template <std::derived_from<ITechnique> T>
    void RenderPassManager::UnregisterTechnique(const std::string& uid)
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
    }

} // namespace nc