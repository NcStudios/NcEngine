#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>

#include "Scene.h"
#include "../../project/Scenes/InitialScene.h"

namespace nc::scene
{
    class SceneManager
    {
        private:
            std::vector<Scene*> activeScenes;
        public:
            SceneManager();
            void LoadScene(Scene* scenePtr);
            void UnloadScene(Scene* scenePtr);
    };
} //end namespace nc::scene


#endif