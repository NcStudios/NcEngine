#pragma once

#include "Scene.h"
#include "shared/SceneHelper.h"

namespace nc::sample
{
    class VulkanScene : public Scene
    {
        public:
            void Load(NcEngine* engine) override;
            void Unload() override;
        private:
            SceneHelper m_sceneHelper;
    };
}