#pragma once

#include "scene/Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class SpawnTest : public Scene
    {
        public:
            void Load(Registry* registry, ModuleProvider modules) override;
            void Unload() override;
        
        public:
            SceneHelper m_sceneHelper;
    };
}