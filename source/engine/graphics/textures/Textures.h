#include "graphics/vk/ImmutableImage.h"

#include <string>

namespace nc::graphics
{
struct TextureBuffer
{
    ImmutableImage image;
    vk::DescriptorImageInfo imageInfo;
    std::string uid;
};
} // namespace nc::graphics