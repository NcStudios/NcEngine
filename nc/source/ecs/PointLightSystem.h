#pragma once

#include "Ecs.h"
#include "SoA.h"
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
            static const uint32_t MAX_POINT_LIGHTS = 10u;

            PointLightSystem(registry_type* registryType, graphics::Graphics2* graphics);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            void Update();
            void Clear();

        private:
            nc::graphics::Graphics2* m_graphics;
            registry_type* m_registry;
            bool m_isSystemDirty;
    };
}