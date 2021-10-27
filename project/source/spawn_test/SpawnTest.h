#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class SpawnTest : public Scene
    {
        public:
            void Load(NcEngine* engine) override;
            void Unload() override;
        
        public:
            SceneHelper m_sceneHelper;
    };
}