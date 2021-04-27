#include "Initializers.h"

namespace nc::graphics::vulkan
{
    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format)
    {
        vk::AttachmentDescription attachmentDescription{};
        attachmentDescription.setFormat(format);
        attachmentDescription.setSamples(vk::SampleCountFlagBits::e1);
        attachmentDescription.setLoadOp(vk::AttachmentLoadOp::eClear);
        attachmentDescription.setStoreOp(vk::AttachmentStoreOp::eStore);
        attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);

        switch (type)
        {
            case AttachmentType::Color:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setInitialLayout(vk::ImageLayout::eUndefined);
                attachmentDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
                break;

            case AttachmentType::Depth:
                attachmentDescription.setStencilLoadOp(vk::AttachmentLoadOp::eClear);
                attachmentDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
                attachmentDescription.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                break;
        }
        return attachmentDescription;
    }

    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex)
    {
        vk::AttachmentReference attachmentReference;
        attachmentReference.setAttachment(attachmentIndex);

        switch (type)
        {
            case AttachmentType::Color:
                attachmentReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
                break;

            case AttachmentType::Depth:
                attachmentReference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                break;
        }
        return attachmentReference;
    }

    vk::SubpassDescription CreateSubpassDescription(vk::AttachmentReference& colorReference, vk::AttachmentReference& depthReference)
    {
        vk::SubpassDescription subpassDescription{};
        subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics); // Vulkan may support compute subpasses later, so explicitly set this to a graphics bind point.
        subpassDescription.setColorAttachmentCount(1);
        subpassDescription.setPColorAttachments(&colorReference);
        subpassDescription.setPDepthStencilAttachment(&depthReference);
        subpassDescription.setInputAttachmentCount(0);
        subpassDescription.setPreserveAttachmentCount(0);
        subpassDescription.setPPreserveAttachments(nullptr);
        subpassDescription.setPResolveAttachments(nullptr);
        return subpassDescription;
    }

    vk::SubpassDependency CreateSubpassDependency(AttachmentType type)
    {
        vk::SubpassDependency subpassDependency;

        switch (type)
        {
            case AttachmentType::Color:
                subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
                subpassDependency.setDstSubpass(0); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
                subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // The type of operation to wait on. (What our dependency is)
                subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // Specifies the type of operation that should do the waiting
                subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting
                break;

            case AttachmentType::Depth:
                subpassDependency.setSrcSubpass(0); // Refers to the implicit subpass prior to the render pass. (Would refer to the one after the render pass if put in setDstSubPass)
                subpassDependency.setDstSubpass(VK_SUBPASS_EXTERNAL); // The index of our subpass. **IMPORTANT. The index of the destination subpass must always be higher than the source subpass to prevent dependency graph cycles. (Unless the source is VK_SUBPASS_EXTERNAL)
                subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput); // The type of operation to wait on. (What our dependency is)
                subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe); // Specifies the type of operation that should do the waiting
                subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);  // Specifies the specific operation that should do the waiting
                subpassDependency.setDependencyFlags(vk::DependencyFlagBits::eByRegion);  // Specifies the specific operation that should do the waiting
                break;
        }
        
        return subpassDependency;      
    }
}