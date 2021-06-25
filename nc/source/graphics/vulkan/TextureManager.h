#pragma once

#include <unordered_map>
#include <string>
#include "vulkan/vk_mem_alloc.hpp"

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
                nc::graphics::Graphics2* m_graphics;
        };
    }
}