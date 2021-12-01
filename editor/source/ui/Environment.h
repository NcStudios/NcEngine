#pragma once

#include "framework/SceneData.h"

namespace nc::editor
{
    class Environment
    {
        public:
            Environment(SceneData* sceneData);

            void Draw();

        private:
            SceneData* m_sceneData;
    };
}