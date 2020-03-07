#pragma once
#include <vector>

namespace nc::scene
{
    class Scene;

    class SceneManager
    {
        public:
            SceneManager();
            void LoadScene(Scene* scenePtr);
            void UnloadScene(Scene* scenePtr);
        private:
            std::vector<Scene*> activeScenes;
    };
} //end namespace nc::scene