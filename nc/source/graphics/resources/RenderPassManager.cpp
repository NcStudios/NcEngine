#include "RenderPassManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Swapchain.h"
#include "optick/optick.h"

#include <iostream>

namespace nc::graphics
{
    RenderPassManager::RenderPassManager(Graphics* graphics, const Vector2& dimensions)
    : m_graphics(graphics),
      m_renderPasses{},
      m_frameBufferAttachments{}
    {
        auto* base = m_graphics->GetBasePtr();
        auto* swapchain = m_graphics->GetSwapchainPtr();

        /** Shadow mapping pass */
        std::array<AttachmentSlot, 1> shadowAttachmentSlots
        {
            CreateAttachmentSlot(0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1)
        };

        std::array<Subpass, 1> shadowSubpasses
        {
            CreateSubpass(shadowAttachmentSlots.at(0))
        };

        Create(RenderPassManager::ShadowMappingPass, shadowAttachmentSlots, shadowSubpasses, ClearValue::Depth, dimensions);

        /** Lit shading pass */
        std::array<AttachmentSlot, 3> litAttachmentSlots
        {
            CreateAttachmentSlot(0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, base->GetMaxSamplesCount()),
            CreateAttachmentSlot(1, AttachmentType::Depth, base->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, base->GetMaxSamplesCount()),
            CreateAttachmentSlot(2, AttachmentType::Resolve, swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1)
        };

        std::array<Subpass, 1> litSubpasses
        {
            CreateSubpass(litAttachmentSlots.at(0), litAttachmentSlots.at(1), litAttachmentSlots.at(2))
        };
        
        Create(RenderPassManager::LitShadingPass, litAttachmentSlots, litSubpasses, ClearValue::DepthAndColor, dimensions);
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
        auto clearValues = CreateClearValues(renderPass->valuesToClear, m_graphics->GetClearColor());
        auto renderPassInfo = CreateRenderPassBeginInfo(renderPass->renderpass.get(), 
                                                        GetFrameBufferAttachment(renderPass->uid, renderTargetIndex).frameBuffer.get(), 
                                                        renderPass->renderTargetSize.extent, 
                                                        clearValues);

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
                                   ClearValue valuesToClear,
                                   const Vector2& dimensions)
    {
        auto* base = m_graphics->GetBasePtr();
        auto* swapchain = m_graphics->GetSwapchainPtr();

        RenderTargetSize renderTargetSize
        {
            .dimensions = dimensions,
            .extent = swapchain->GetExtent()
        };

        RenderPass renderpass
        {
            .uid = uid,
            .renderTargetSize = renderTargetSize,
            .renderpass = CreateRenderPass(base->GetDevice(), attachmentSlots, subpasses),
            .valuesToClear = valuesToClear,
            .techniques = {}
        };

        m_renderPasses.push_back(std::move(renderpass));
    }

    void RenderPassManager::Resize(const Vector2& dimensions, vk::Extent2D extent)
    {
        auto* base = m_graphics->GetBasePtr();

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
            frameBufferAttachment.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);
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

        auto* base = m_graphics->GetBasePtr();
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

        frameBufferAttachment.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);
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

        auto* base = m_graphics->GetBasePtr();
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

        frameBufferAttachment.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);
        m_frameBufferAttachments.push_back(std::move(frameBufferAttachment));
    }
}