#include "RenderPassManager.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Swapchain.h"

#include <iostream>

namespace nc::graphics
{
    RenderPassManager::RenderPassManager(Graphics* graphics, const Vector2& dimensions)
    : m_graphics(graphics),
      m_renderPasses{},
      m_renderTargets{}
    {
        auto* base = m_graphics->GetBasePtr();
        auto* swapchain = m_graphics->GetSwapchainPtr();

        /** Shadow mapping pass */
        std::array<AttachmentSlot, 1> shadowAttachmentSlots
        {
            CreateAttachmentSlot(0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore)
        };

        std::array<Subpass, 1> shadowSubpasses
        {
            CreateSubpass(shadowAttachmentSlots.at(0))
        };

        Create(RenderPassManager::ShadowMappingPass, shadowAttachmentSlots, shadowSubpasses, ClearValue::Depth, dimensions);

        /** Lit shading pass */
        std::array<AttachmentSlot, 2> litAttachmentSlots
        {
            CreateAttachmentSlot(0, AttachmentType::Color, swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore),
            CreateAttachmentSlot(1, AttachmentType::Depth, base->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare)
        };

        std::array<Subpass, 1> litSubpasses
        {
            CreateSubpass(litAttachmentSlots.at(1), litAttachmentSlots.at(0))
        };
        
        Create(RenderPassManager::LitShadingPass, litAttachmentSlots, litSubpasses, ClearValue::DepthAndColor, dimensions);
    }

    RenderPassManager::~RenderPassManager()
    {
        m_renderPasses.clear();
        m_renderTargets.clear();
    }

    void RenderPassManager::Execute(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex, const PerFrameRenderState& frameData)
    {
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
                                                        GetRenderTarget(renderPass->uid, renderTargetIndex).frameBuffer.get(), 
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
            .renderpass = std::move(CreateRenderPass(base, attachmentSlots, subpasses)),
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

        for (auto& renderTarget : m_renderTargets)
        {
            auto& renderPass = Acquire(renderTarget.renderPassUid);

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.setRenderPass(renderPass.renderpass.get());
            framebufferInfo.setAttachmentCount(renderTarget.attachmentHandles.size());
            framebufferInfo.setPAttachments(renderTarget.attachmentHandles.data());
            framebufferInfo.setWidth(dimensions.x);
            framebufferInfo.setHeight(dimensions.y);
            framebufferInfo.setLayers(1);

            renderTarget.frameBuffer.reset();
            renderTarget.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);
        }
    }

    RenderTarget& RenderPassManager::GetRenderTarget(const std::string& uid, uint32_t index)
    {
        auto renderTargetPos = std::ranges::find_if(m_renderTargets, [uid, index](const auto& aRenderTarget)
        {
            return (aRenderTarget.renderPassUid == uid && aRenderTarget.index == index);
        });

        if (renderTargetPos == m_renderTargets.end())
        {
            throw std::runtime_error("RenderPassManager::GetRenderTarget - RenderTarget does not exist.");
        }

        return *renderTargetPos;
    }

    void RenderPassManager::RegisterAttachments(std::vector<vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index)
    {
        auto renderTargetPos = std::ranges::find_if(m_renderTargets, [uid, index](auto& aRenderTarget)
        {
            return (aRenderTarget.renderPassUid == uid && aRenderTarget.index == index);
        });

        if (renderTargetPos != m_renderTargets.end())
        {
            *renderTargetPos = std::move(m_renderTargets.back());
            m_renderTargets.pop_back();
        }

        auto* base = m_graphics->GetBasePtr();
        auto renderTarget = RenderTarget{};

        renderTarget.attachmentHandles.reserve(attachmentHandles.size());
        for (auto attachmentHandle : attachmentHandles)
        {
            renderTarget.attachmentHandles.push_back(attachmentHandle);
        }
        renderTarget.renderPassUid = uid;
        renderTarget.index = index;

        auto& renderpass = Acquire(uid);
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderpass.renderpass.get());
        framebufferInfo.setAttachmentCount(renderTarget.attachmentHandles.size());
        framebufferInfo.setPAttachments(renderTarget.attachmentHandles.data());
        framebufferInfo.setWidth(renderpass.renderTargetSize.dimensions.x);
        framebufferInfo.setHeight(renderpass.renderTargetSize.dimensions.y);
        framebufferInfo.setLayers(1);

        renderTarget.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);

        m_renderTargets.push_back(std::move(renderTarget));
    }

    void RenderPassManager::RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid)
    {
        auto renderTargetPos = std::ranges::find_if(m_renderTargets, [uid](auto& aRenderTarget)
        {
            return (aRenderTarget.renderPassUid == uid);
        });

        if (renderTargetPos != m_renderTargets.end())
        {
            *renderTargetPos = std::move(m_renderTargets.back());
            m_renderTargets.pop_back();
        }

        auto* base = m_graphics->GetBasePtr();
        auto renderTarget = RenderTarget{};

        renderTarget.attachmentHandles.reserve(1);
        renderTarget.attachmentHandles.push_back(attachmentHandle);
        renderTarget.renderPassUid = uid;
        renderTarget.index = 0;

        auto& renderpass = Acquire(uid);
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderpass.renderpass.get());
        framebufferInfo.setAttachmentCount(1);
        framebufferInfo.setPAttachments(renderTarget.attachmentHandles.data());
        framebufferInfo.setWidth(renderpass.renderTargetSize.dimensions.x);
        framebufferInfo.setHeight(renderpass.renderTargetSize.dimensions.y);
        framebufferInfo.setLayers(1);

        renderTarget.frameBuffer = base->GetDevice().createFramebufferUnique(framebufferInfo);

        m_renderTargets.push_back(std::move(renderTarget));
    }

}