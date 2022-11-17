#include "RenderPassManager.h"
#include "graphics/GpuOptions.h"
#include "graphics/vk/Swapchain.h"
#include "optick/optick.h"

#include <iostream>

namespace
{
constexpr std::array<float, 4> ClearColor = {0.1f, 0.1f, 0.1f, 0.1f};

auto CreateClearValues(nc::graphics::ClearValueFlags_t clearFlags) -> std::vector<vk::ClearValue>
{
    std::vector<vk::ClearValue> clearValues;

    if(clearFlags & nc::graphics::ClearValueFlags::Color)
    {
        clearValues.push_back(vk::ClearValue{vk::ClearColorValue{ClearColor}});
    }

    if(clearFlags & nc::graphics::ClearValueFlags::Depth)
    {
        clearValues.push_back(vk::ClearValue{vk::ClearDepthStencilValue{1.0f, 0}});
    }

    return clearValues;
}
}

namespace nc::graphics
{

    RenderPassManager::RenderPassManager(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, ShaderDescriptorSets* descriptorSets, const Vector2& dimensions)
        : m_device{device},
          m_swapchain{swapchain},
          m_gpuOptions{gpuOptions},
          m_descriptorSets{descriptorSets},
          m_renderPasses{},
          m_frameBufferAttachments{}
    {
        /** Shadow mapping pass */
        std::array<AttachmentSlot, 1> shadowAttachmentSlots
        {
            AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
        };

        std::array<Subpass, 1> shadowSubpasses
        {
            Subpass{shadowAttachmentSlots[0]}
        };

        Create(RenderPassManager::ShadowMappingPass, shadowAttachmentSlots, shadowSubpasses, ClearValueFlags::Depth, dimensions);

        /** Lit shading pass */
        std::array<AttachmentSlot, 3> litAttachmentSlots
        {
            AttachmentSlot{0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, gpuOptions->GetMaxSamplesCount()},
            AttachmentSlot{1, AttachmentType::Depth, gpuOptions->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, gpuOptions->GetMaxSamplesCount()},
            AttachmentSlot{2, AttachmentType::Resolve, swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
        };

        std::array<Subpass, 1> litSubpasses
        {
            Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
        };

        Create(RenderPassManager::LitShadingPass, litAttachmentSlots, litSubpasses, ClearValueFlags::Depth | ClearValueFlags::Color, dimensions);
    }

    RenderPassManager::~RenderPassManager() noexcept
    {
        m_renderPasses.clear();
        m_frameBufferAttachments.clear();
    }

    void RenderPassManager::Execute(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("RenderPassManager::Execute", Optick::Category::Rendering);
        auto& renderPass = Acquire(uid);

        Begin(&renderPass, cmd, renderTargetIndex);

        for (auto& technique : renderPass.techniques)
        {
            if (!technique->CanBind(frameData)) continue;

            technique->Bind(cmd);

            if (!technique->CanRecord(frameData)) continue;

            technique->Record(cmd, frameData);
        }

        End(cmd);
    }

    void RenderPassManager::Begin(RenderPass* renderPass, vk::CommandBuffer* cmd, uint32_t renderTargetIndex)
    {
        const auto clearValues = CreateClearValues(renderPass->clearFlags);
        const auto renderPassInfo = vk::RenderPassBeginInfo
        {
            renderPass->renderpass.get(), GetFrameBufferAttachment(renderPass->uid, renderTargetIndex).frameBuffer.get(),
            vk::Rect2D{vk::Offset2D{0, 0}, renderPass->renderTargetSize.extent},
            static_cast<uint32_t>(clearValues.size()),
            clearValues.data()
        };

        cmd->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    }

    RenderPass& RenderPassManager::Acquire(const std::string& uid)
    {
        auto renderPassPos = std::ranges::find_if(m_renderPasses, [uid](const auto& aRenderPass)
        {
            return (aRenderPass.uid == uid);
        });

        if (renderPassPos == m_renderPasses.end())
        {
            throw std::runtime_error("RenderPassManager::Acquire - RenderPass does not exist.");
        }

        return *renderPassPos;
    }

    void RenderPassManager::End(vk::CommandBuffer* cmd)
    {
        cmd->endRenderPass();
    }

    void RenderPassManager::Create(const std::string& uid,
                                   std::span<const AttachmentSlot> attachmentSlots,
                                   std::span<const Subpass> subpasses,
                                   ClearValueFlags_t clearFlags,
                                   const Vector2& dimensions)
    {
        const auto size = RenderTargetSize{dimensions, m_swapchain->GetExtent()};
        m_renderPasses.emplace_back(attachmentSlots, subpasses, m_device, size, uid, clearFlags);
    }

    void RenderPassManager::Resize(const Vector2& dimensions, vk::Extent2D extent)
    {
        for (auto& renderPass : m_renderPasses)
        {
            renderPass.renderTargetSize.dimensions = dimensions;
            renderPass.renderTargetSize.extent = extent;
        }

        for (auto& frameBufferAttachment : m_frameBufferAttachments)
        {
            auto& renderPass = Acquire(frameBufferAttachment.renderPassUid);

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.setRenderPass(renderPass.renderpass.get());
            framebufferInfo.setAttachmentCount(static_cast<uint32_t>(frameBufferAttachment.attachmentHandles.size()));
            framebufferInfo.setPAttachments(frameBufferAttachment.attachmentHandles.data());
            framebufferInfo.setWidth(static_cast<uint32_t>(dimensions.x));
            framebufferInfo.setHeight(static_cast<uint32_t>(dimensions.y));
            framebufferInfo.setLayers(1);

            frameBufferAttachment.frameBuffer.reset();
            frameBufferAttachment.frameBuffer = m_device.createFramebufferUnique(framebufferInfo);
        }
    }

    FrameBufferAttachment& RenderPassManager::GetFrameBufferAttachment(const std::string& uid, uint32_t index)
    {
        auto frameBufferPos = std::ranges::find_if(m_frameBufferAttachments, [&uid, index](const auto& frameBufferAttachment)
        {
            return (frameBufferAttachment.index == index && frameBufferAttachment.renderPassUid == uid);
        });

        if (frameBufferPos == m_frameBufferAttachments.end())
        {
            throw std::runtime_error("RenderPassManager::GetFrameBufferAttachment - FrameBufferAttachment does not exist.");
        }

        return *frameBufferPos;
    }

    void RenderPassManager::RegisterAttachments(std::vector<vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index)
    {
        auto frameBufferPos = std::ranges::find_if(m_frameBufferAttachments, [&uid, index](const auto& frameBufferAttachment)
        {
            return (frameBufferAttachment.index == index && frameBufferAttachment.renderPassUid == uid);
        });

        if (frameBufferPos != m_frameBufferAttachments.end())
        {
            *frameBufferPos = std::move(m_frameBufferAttachments.back());
            m_frameBufferAttachments.pop_back();
        }

        auto frameBufferAttachment = FrameBufferAttachment{};

        frameBufferAttachment.attachmentHandles = std::move(attachmentHandles);
        frameBufferAttachment.renderPassUid = uid;
        frameBufferAttachment.index = index;

        auto& renderpass = Acquire(uid);
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderpass.renderpass.get());
        framebufferInfo.setAttachmentCount(static_cast<uint32_t>(frameBufferAttachment.attachmentHandles.size()));
        framebufferInfo.setPAttachments(frameBufferAttachment.attachmentHandles.data());
        framebufferInfo.setWidth(static_cast<uint32_t>(renderpass.renderTargetSize.dimensions.x));
        framebufferInfo.setHeight(static_cast<uint32_t>(renderpass.renderTargetSize.dimensions.y));
        framebufferInfo.setLayers(1);

        frameBufferAttachment.frameBuffer = m_device.createFramebufferUnique(framebufferInfo);
        m_frameBufferAttachments.push_back(std::move(frameBufferAttachment));
    }

    void RenderPassManager::RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid)
    {
        auto frameBufferPos = std::ranges::find_if(m_frameBufferAttachments, [uid](const auto& frameBufferAttachment)
        {
            return (frameBufferAttachment.renderPassUid == uid);
        });

        if (frameBufferPos != m_frameBufferAttachments.end())
        {
            *frameBufferPos = std::move(m_frameBufferAttachments.back());
            m_frameBufferAttachments.pop_back();
        }

        auto frameBufferAttachment = FrameBufferAttachment{};

        frameBufferAttachment.attachmentHandles.reserve(1);
        frameBufferAttachment.attachmentHandles.push_back(attachmentHandle);
        frameBufferAttachment.renderPassUid = uid;
        frameBufferAttachment.index = 0;

        auto& renderpass = Acquire(uid);
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderpass.renderpass.get());
        framebufferInfo.setAttachmentCount(1);
        framebufferInfo.setPAttachments(frameBufferAttachment.attachmentHandles.data());
        framebufferInfo.setWidth(static_cast<uint32_t>(renderpass.renderTargetSize.dimensions.x));
        framebufferInfo.setHeight(static_cast<uint32_t>(renderpass.renderTargetSize.dimensions.y));
        framebufferInfo.setLayers(1);

        frameBufferAttachment.frameBuffer = m_device.createFramebufferUnique(framebufferInfo);
        m_frameBufferAttachments.push_back(std::move(frameBufferAttachment));
    }
}