#pragma once

#include "Ecs.h"
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
            MeshRendererSystem(registry_type* registry, graphics::Graphics2* graphics);

            void Add(vulkan::MeshRenderer& meshRenderer);
            void Remove(Entity entity);
            void Clear();
            void Update();

        private:
            nc::graphics::Graphics2* m_graphics;
            registry_type* m_registry;
    };
}