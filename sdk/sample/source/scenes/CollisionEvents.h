#pragma once

#include "scene/Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class CollisionEvents : public Scene
    {
        public:
            void Load(Registry* registry, ModuleProvider modules) override;
            void Unload() override;

        private:
            SceneHelper m_sceneHelper;
    };
}