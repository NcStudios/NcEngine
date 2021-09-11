#include "RenderPass.h"

namespace nc::graphics
{
    RenderPass::RenderPass(Graphics* graphics, 
                           const std::vector<vk::SubpassDependency>& subpassDependencies,
                           const std::vector<Attachment>& attachments)
    {
        auto device = m_graphics->GetBasePtr()->GetDevice();

        auto attachmentDescriptions = std::vector<vk::AttachmentDescription>();
        attachmentDescriptions.reserve(attachments.size());
        std::transform(attachments.cbegin(), attachments.cend(), std::back_inserter(attachmentDescriptions), [](const auto& attachment))
        {
            return attachment.description;
        }



        auto createInfo = CreateRenderPassCreateInfo(const std::vector<vk::AttachmentDescription>& attachmentDescriptions, const std::vector<vk::SubpassDescription>& subpassDescriptions, const std::vector<vk::SubpassDependency>& subpassDependencies)
    }

    void Begin(vk::CommandBuffer* cmd);
    void End(vk::CommandBuffer* cmd);
}