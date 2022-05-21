#pragma once

namespace nc
{
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
            PointLightSystem& operator = (PointLightSystem&&) = delete;
            PointLightSystem& operator = (const PointLightSystem&) = delete;

            bool CheckDirtyAndReset();
            void Clear();

        private:
            Registry* m_registry;
            bool m_isSystemDirty;
    };
}