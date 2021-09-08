#pragma once

#include <unordered_map>
#include <string>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class TextureManager
    {
        public:
            TextureManager(nc::graphics::Graphics* graphics);
            void LoadTextures(const std::vector<std::string>& paths);
            
        private:
            nc::graphics::Graphics* m_graphics;
    };
}