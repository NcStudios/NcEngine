#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class ClickEvents : public scene::Scene
    {
        public:
            void Load() override;
            void Unload() override;

        private:
            SceneHelper m_sceneHelper;
    };
}