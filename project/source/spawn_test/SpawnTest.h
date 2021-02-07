#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class SpawnTest : public scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
        
        public:
            SceneHelper m_sceneHelper;
    };
}