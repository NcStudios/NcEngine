#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class JointsTest : public Scene
    {
        public:
            void Load(NcEngine* registry) override;
            void Unload() override;
        
        private:
            SceneHelper m_sceneHelper;
    };
}