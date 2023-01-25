#include "RenderPasses.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuOptions.h"
#include "graphics/Swapchain.h"
#include "graphics/renderpasses/RenderTarget.h"
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

inline static const std::string LitShadingPass = "Lit Pass";

namespace nc::graphics
{
    RenderPasses::RenderPasses(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, const Vector2& dimensions)
        : m_device{device},
          m_swapchain{swapchain},
          m_gpuOptions{gpuOptions},
          m_descriptorSets{descriptorSets},
          m_renderPasses{},
          m_depthStencil{ std::make_unique<RenderTarget>(device, gpuAllocator, dimensions, true, m_gpuOptions->GetMaxSamplesCount(), m_gpuOptions->GetDepthFormat()) },
          m_colorBuffer{ std::make_unique<RenderTarget>(device, gpuAllocator, dimensions, false, m_gpuOptions->GetMaxSamplesCount(), m_swapchain->GetFormat()) },
          m_frameBufferAttachments{}
    {
        /** Lit shading pass */
        std::array<AttachmentSlot, 3> litAttachmentSlots
        {
            AttachmentSlot{0, AttachmentType::Color, m_swapchain->GetFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, m_gpuOptions->GetMaxSamplesCount()},
            AttachmentSlot{1, AttachmentType::Depth, m_gpuOptions->GetDepthFormat(), vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, m_gpuOptions->GetMaxSamplesCount()},
            AttachmentSlot{2, AttachmentType::Resolve, m_swapchain->GetFormat(), vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
        };

        std::array<Subpass, 1> litSubpasses
        {
            Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
        };

        Add(LitShadingPass, litAttachmentSlots, litSubpasses, ClearValueFlags::Depth | ClearValueFlags::Color, dimensions);

        auto& colorImageViews = m_swapchain->GetColorImageViews();
        auto& depthImageView = m_depthStencil->GetImageView();
        auto& colorResolveView = m_colorBuffer->GetImageView();

        uint32_t index = 0;
        for (auto& imageView : colorImageViews) 
        {
            RegisterAttachments(std::vector<vk::ImageView>{colorResolveView, depthImageView, imageView}, LitShadingPass, index++); 
        }

        #ifdef NC_EDITOR_ENABLED
        RegisterTechnique<WireframeTechnique>(LitShadingPass);
        #endif

        RegisterTechnique<EnvironmentTechnique>(LitShadingPass);
        RegisterTechnique<PbrTechnique>(LitShadingPass);
        RegisterTechnique<ParticleTechnique>(LitShadingPass);
        RegisterTechnique<UiTechnique>(LitShadingPass);
    }

    RenderPasses::~RenderPasses() noexcept
    {
        m_renderPasses.clear();
        m_frameBufferAttachments.clear();
    }

    void RenderPasses::Execute(const std::string& uid, vk::CommandBuffer* cmd, uint32_t renderTargetIndex, const PerFrameRenderState& frameData)
    {
        OPTICK_CATEGORY("RenderPasses::Execute", Optick::Category::Rendering);
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

    void RenderPasses::Begin(RenderPass* renderPass, vk::CommandBuffer* cmd, uint32_t renderTargetIndex)
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

    RenderPass& RenderPasses::Acquire(const std::string& uid)
    {
        auto renderPassPos = std::ranges::find_if(m_renderPasses, [uid](const auto& aRenderPass)
        {
            return (aRenderPass.uid == uid);
        });

        if (renderPassPos == m_renderPasses.end())
        {
            throw std::runtime_error("RenderPasses::Acquire - RenderPass does not exist.");
        }

        return *renderPassPos;
    }

    void RenderPasses::End(vk::CommandBuffer* cmd)
    {
        cmd->endRenderPass();
    }

    void RenderPasses::Add(const std::string& uid,
                                std::span<const AttachmentSlot> attachmentSlots,
                                std::span<const Subpass> subpasses,
                                ClearValueFlags_t clearFlags,
                                const Vector2& dimensions)
    {
        const auto size = RenderTargetSize{dimensions, m_swapchain->GetExtent()};
        m_renderPasses.emplace_back(attachmentSlots, subpasses, m_device, size, uid, clearFlags);
    }

    void RenderPasses::Remove(const std::string& uid)
    {
        const auto it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), ([uid](const auto& pass){return pass.uid == uid;}));

        if(it == m_renderPasses.end())
            throw NcError("Render pass does not exist: " + uid);

        *it = std::move(m_renderPasses.back());
        m_renderPasses.pop_back();
    }

    void RenderPasses::Resize(const Vector2& dimensions, vk::Extent2D extent)
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

    FrameBufferAttachment& RenderPasses::GetFrameBufferAttachment(const std::string& uid, uint32_t index)
    {
        auto frameBufferPos = std::ranges::find_if(m_frameBufferAttachments, [&uid, index](const auto& frameBufferAttachment)
        {
            return (frameBufferAttachment.index == index && frameBufferAttachment.renderPassUid == uid);
        });

        if (frameBufferPos == m_frameBufferAttachments.end())
        {
            throw std::runtime_error("RenderPasses::GetFrameBufferAttachment - FrameBufferAttachment does not exist.");
        }

        return *frameBufferPos;
    }

    void RenderPasses::RegisterAttachments(std::span<const vk::ImageView> attachmentHandles, const std::string& uid, uint32_t index)
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

    void RenderPasses::RegisterAttachment(vk::ImageView attachmentHandle, const std::string& uid)
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

        auto handles = std::vector<vk::ImageView>{attachmentHandle};
        frameBufferAttachment.attachmentHandles = handles;
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