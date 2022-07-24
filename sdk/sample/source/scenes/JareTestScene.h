#pragma once

#include "scene/Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class JareTestScene : public Scene
    {
        public:
            void Load(Registry* registry, ModuleRegistry* modules) override;
            void Unload() override;
        private:
            SceneHelper m_sceneHelper;
    };
}