#pragma once

#include "SoA.h"

#include <unordered_map>

namespace nc
{
    class Registry;
    namespace graphics { class Graphics; }
}

namespace nc::ecs
{
    class PointLightSystem
    {
        public:
            PointLightSystem(Registry* registryType, graphics::Graphics* graphics);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            bool CheckDirtyAndReset();
            void Clear();

        private:
            graphics::Graphics* m_graphics;
            Registry* m_registry;
            bool m_isSystemDirty;
    };
}