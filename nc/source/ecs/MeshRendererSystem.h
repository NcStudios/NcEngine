#pragma once

#include "Ecs.h"
#include "component/MeshRenderer.h"
#include "graphics/MeshManager.h"
#include "graphics/TextureManager.h"

namespace nc::graphics 
{
    class Graphics;
}

namespace nc::ecs
{
    class MeshRendererSystem
    {
        public:
            MeshRendererSystem(registry_type* registry, graphics::Graphics* graphics);

            void Add(MeshRenderer& meshRenderer);
            void Remove(Entity entity);
            void Clear();
            void Update();

        private:
            nc::graphics::Graphics* m_graphics;
            registry_type* m_registry;
    };
}