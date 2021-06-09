#pragma once

#include "component/vulkan/MeshRenderer.h"
#include "graphics/vulkan/MeshManager.h"
#include "graphics/vulkan/TextureManager.h"

namespace nc::graphics 
{
    class Graphics2;
}

namespace nc::ecs
{
    class MeshRendererSystem
    {
        public:
            MeshRendererSystem(graphics::Graphics2* graphics);

            void Add(vulkan::MeshRenderer& meshRenderer);
            void Remove(EntityHandle entity);

        private:
            nc::graphics::Graphics2* m_graphics;
    };
}