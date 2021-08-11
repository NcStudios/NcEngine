#pragma once

#include "Ecs.h"
#include "component/vulkan/DebugWidget.h"
#include "graphics/vulkan/MeshManager.h"

namespace nc::graphics 
{
    class Graphics2;
}

namespace nc::ecs
{
    class DebugWidgetSystem
    {
        public:
            DebugWidgetSystem(registry_type* registry, graphics::Graphics2* graphics);

            void Add(vulkan::DebugWidget& debugWidget);
            void Remove(Entity entity);
            void Clear();            

        private:
            nc::graphics::Graphics2* m_graphics;
    };
}