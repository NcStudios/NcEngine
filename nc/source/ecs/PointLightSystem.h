#pragma once

#include "Ecs.h"
#include "ComponentSystem.h"
#include "component/vulkan/PointLight.h"

namespace nc::graphics
{
    class Graphics2;
    namespace vulkan { class Commands; }
}

namespace nc::ecs
{
    class PointLightSystem
    {
        public:
            static const uint32_t MAX_POINT_LIGHTS = 5u;

            PointLightSystem(graphics::Graphics2* graphics, uint32_t maxPointLights);
            ComponentSystem<vulkan::PointLight>* GetSystem();
            vulkan::PointLight* Add(EntityHandle parentHandle);
            vulkan::PointLight* Add(EntityHandle parentHandle, vulkan::PointLightInfo info);
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            vulkan::PointLight* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<vulkan::PointLight>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<vulkan::PointLight> m_componentSystem;
            nc::graphics::Graphics2* m_graphics;
    };
}