#pragma once

#include "Scene.h"

#include <memory>

namespace nc::scene
{
    class SceneManagerImpl
    {
        public:
            SceneManagerImpl();
            
            void QueueSceneChange(std::unique_ptr<Scene>&& swapScene);
            bool IsSceneChangeScheduled() const;
            void LoadActiveScene();
            void UnloadActiveScene();
            void DoSceneChange();

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            bool m_isSceneChangeScheduled;
    };
}