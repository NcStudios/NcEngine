#include "graphics/resources/ImmutableImage.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
struct TextureBuffer
{
    ImmutableImage image;
    vk::DescriptorImageInfo imageInfo;
    std::string uid;
};
} // namespace nc::graphics