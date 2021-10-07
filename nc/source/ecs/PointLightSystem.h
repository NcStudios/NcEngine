#pragma once

#include "component/PointLight.h"
#include "Ecs.h"
#include "SoA.h"

#include <unordered_map>

namespace nc::graphics
{
    class Graphics;
}

namespace nc::ecs
{
    class PointLightSystem
    {
        public:
            PointLightSystem(registry_type* registryType, graphics::Graphics* graphics, uint32_t maxPointLights);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            bool CheckDirtyAndReset();
            void Clear();

        private:
            graphics::Graphics* m_graphics;
            registry_type* m_registry;
            uint32_t m_maxPointLights;
            bool m_isSystemDirty;
    };
}