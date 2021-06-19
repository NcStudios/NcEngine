#pragma once

#include "Ecs.h"
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

            PointLightSystem(registry_type* registryType, graphics::Graphics2* graphics, uint32_t maxPointLights);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            void Update();
            void Clear();
            void Add(vulkan::PointLight& pointLight);
            void Remove(Entity entity);

        private:
            std::vector<vulkan::PointLightInfo> m_pointLightInfos;
            nc::graphics::Graphics2* m_graphics;
            registry_type* m_registry;
    };
}