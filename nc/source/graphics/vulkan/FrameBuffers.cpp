#include "FrameBuffers.h"
#include "Device.h"
#include "RenderPass.h"

namespace nc::graphics::vulkan
{
    FrameBuffers::FrameBuffers(const Device* device, const RenderPass* renderpass)
    : m_framebuffers{}
    {
        auto swapChainImageViewsCount = device->GetSwapChainImageViews()->size();
        m_framebuffers.resize(swapChainImageViewsCount);

        for (size_t i = 0; i < swapChainImageViewsCount; i++)
        {
            vk::ImageView attachments[] = { device->GetSwapChainImageViews()->at(i) };

            vk::FramebufferCreateInfo framebufferInfo{};
            framebufferInfo.setRenderPass(*(renderpass->GetRenderPass()));
            framebufferInfo.setAttachmentCount(1);
            framebufferInfo.setPAttachments(attachments);
            framebufferInfo.setWidth(device->GetSwapChainExtentDimensions().x);
            framebufferInfo.setHeight(device->GetSwapChainExtentDimensions().y);
            framebufferInfo.setLayers(1);

            m_framebuffers.emplace_back(device->GetDevice()->createFramebufferUnique(framebufferInfo));
        }


    }

}