#pragma once

#include "Scene.h"

#include <memory>

namespace nc::scene
{
    class SceneManager
    {
        public:
            SceneManager();
            ~SceneManager();

            static void QueueSceneChange(std::unique_ptr<Scene> swapScene);
            
            void QueueSceneChange_(std::unique_ptr<Scene> swapScene);
            bool IsSceneChangeScheduled() const;
            void LoadActiveScene();
            void UnloadActiveScene();
            void DoSceneChange();

        private:
            static SceneManager* m_instance;
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            bool m_isSceneChangeScheduled;
    };
}