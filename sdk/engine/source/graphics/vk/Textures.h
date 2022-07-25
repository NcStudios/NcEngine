#include "graphics/resources/ImmutableImage.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
struct TextureBuffer
{
    ImmutableImage image;
    std::string uid;
};

struct TextureImageInfo
{
    vk::DescriptorImageInfo imageInfo;
};
}