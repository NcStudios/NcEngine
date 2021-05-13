#pragma once

#include <unordered_map>
#include <string>
#include "vulkan/vulkan.hpp"

namespace nc::graphics
{
    class Graphics2;

    namespace vulkan
    {
        struct TexturesData;

        class TextureManager
        {
            public:
                TextureManager(nc::graphics::Graphics2* graphics);
                void LoadTextures(const std::vector<std::string>& paths);
                
            private:
                void CreateDescriptorSet(TexturesData* textures);
                void WriteDescriptorImages(TexturesData* textures, uint32_t arraySize);
                vk::DescriptorImageInfo CreateDescriptorImageInfo(const vk::Sampler& sampler, const vk::ImageView& imageView, vk::ImageLayout layout);

                nc::graphics::Graphics2* m_graphics;
        };
    }
}