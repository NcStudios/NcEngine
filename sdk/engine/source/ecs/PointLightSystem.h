#pragma once

#include "utility/Signal.h"

namespace nc
{
    class Entity;
    class PointLight;
    class Registry;
}

namespace nc::ecs
{
    class PointLightSystem
    {
        public:
            PointLightSystem(Registry* registry);
            PointLightSystem(PointLightSystem&&) = delete;
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator=(PointLightSystem&&) = delete;
            PointLightSystem& operator=(const PointLightSystem&) = delete;

            bool CheckDirtyAndReset();
            void Clear();

        private:
            Connection<PointLight&> m_onAddConnection;
            Connection<Entity> m_onRemoveConnection;
            bool m_isSystemDirty;
    };
}