#include "FrameBuffers.h"
#include "Device.h"
#include "RenderPass.h"

namespace nc::graphics::vulkan
{
    FrameBuffers::FrameBuffers(const Device& device, const RenderPass& renderpass)
    : m_framebuffers{}
    {
        auto swapChainImageViewsCount = device.GetSwapChainImageViews().size();
        m_framebuffers.resize(swapChainImageViewsCount);

        for (size_t i = 0; i < swapChainImageViewsCount; i++)
        {
            vk::ImageView attachments[] = { device.GetSwapChainImageViews().at(i) };

            auto swapChainDimensions = device.GetSwapChainExtentDimensions();

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.setRenderPass(renderpass.GetRenderPass());
            framebufferInfo.setAttachmentCount(1);
            framebufferInfo.setPAttachments(attachments);
            framebufferInfo.setWidth(swapChainDimensions.x);
            framebufferInfo.setHeight(swapChainDimensions.y);
            framebufferInfo.setLayers(1);
            m_framebuffers[i] = device.GetDevice().createFramebufferUnique(framebufferInfo);
        }
    }

    const vk::Framebuffer& FrameBuffers::GetFrameBuffer(uint32_t index) const
    {
        return m_framebuffers.at(index).get();
    }
}