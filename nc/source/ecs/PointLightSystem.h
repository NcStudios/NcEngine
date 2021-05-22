#pragma once

#include "Ecs.h"
#include "ComponentSystem.h"
#include "component/vulkan/PointLight.h"

#include <unordered_map>

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
            static const uint32_t MAX_POINT_LIGHTS = 50u;

            PointLightSystem(graphics::Graphics2* graphics, uint32_t maxPointLights);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            ComponentSystem<vulkan::PointLight>* GetSystem();
            vulkan::PointLight* Add(EntityHandle parentHandle, vulkan::PointLightInfo info);
            void Update();
            bool Remove(EntityHandle parentHandle);
            bool Contains(EntityHandle parentHandle) const;
            vulkan::PointLight* GetPointerTo(EntityHandle parentHandle);
            auto GetComponents() -> ComponentSystem<vulkan::PointLight>::ContainerType&;
            void Clear();

        private:
            ComponentSystem<vulkan::PointLight> m_componentSystem;
            std::vector<vulkan::PointLightInfo> m_pointLightInfos;
            nc::graphics::Graphics2* m_graphics;
    };
}