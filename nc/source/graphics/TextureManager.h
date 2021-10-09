#pragma once

#include "graphics/resources/GraphicsResources.h"

#include <unordered_map>
#include <string>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
    class Graphics;

    class TextureManager
    {
        public:
            TextureManager(Graphics* graphics);
            void LoadTextures(const std::vector<std::string>& paths);
            void LoadTexture(const std::string& path);
            
        private:
            Texture CreateTexture(const std::string& path, vk::Sampler* sampler);
            Graphics* m_graphics;
    };
}