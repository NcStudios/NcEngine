#pragma once

#include "Scene.h"
#include "scenes/SceneHelper.h"

namespace project
{
    class MenuScene : public nc::scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;
        
        private:
            SceneHelper m_sceneHelper;
    };
}