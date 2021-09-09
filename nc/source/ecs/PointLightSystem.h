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
            PointLightSystem(registry_type* registryType, graphics::Graphics* graphics);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            void Update();
            void Clear();

        private:
            graphics::Graphics* m_graphics;
            registry_type* m_registry;
            bool m_isSystemDirty;
    };
}