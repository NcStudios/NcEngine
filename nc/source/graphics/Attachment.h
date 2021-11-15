#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>

namespace nc::graphics
{
    struct AttachmentSlot
    {
        vk::AttachmentDescription description;
        vk::UniqueImageView imageView;
        vk::AttachmentReference reference;
    };
}