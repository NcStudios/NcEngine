#pragma once

#include "framework/Callbacks.h"

#include <functional>
#include <string>
#include <vector>

namespace nc::editor
{
    class SceneSelectWidget
    {
        public:
            SceneSelectWidget(SceneCallbacks sceneCallbacks);

            void Draw();
            void UpdateScenes(std::vector<std::string> scenes, int selectedScene);
            
        private:
            std::vector<std::string> m_scenes;
            SceneCallbacks m_callbacks;
            int m_selectedScene;
    };
}