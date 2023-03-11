#pragma once

#include "utility/Signal.h"

namespace nc
{
    class Entity;
    class Registry;
    namespace graphics { class PointLight; }
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
            Connection<graphics::PointLight&> m_onAddConnection;
            Connection<Entity> m_onRemoveConnection;
            bool m_isSystemDirty;
    };
}