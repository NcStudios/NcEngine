#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>

namespace nc::graphics
{
    struct Attachment
    {
        vk::AttachmentDescription description;
        vk::UniqueImageView imageView;
        vk::AttachmentReference reference;
    };
}