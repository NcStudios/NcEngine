#pragma once

#include "Scene.h"
#include "scenes/SceneHelper.h"

#include <memory>
#include <string>

namespace project
{
    class SpawnTest : public nc::scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
        
        public:
            SceneHelper m_sceneHelper;
    };
}