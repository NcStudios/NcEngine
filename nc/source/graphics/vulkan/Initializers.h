#pragma once

#include "vulkan/vulkan.hpp"

namespace nc::graphics::vulkan
{
    enum class AttachmentType : uint8_t
    {
        Color,
        Depth
    };

    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format);
    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex);
    vk::SubpassDescription CreateSubpassDescription(vk::AttachmentReference& colorReference, vk::AttachmentReference& depthReference);
    vk::SubpassDependency CreateSubpassDependency(AttachmentType type);
}