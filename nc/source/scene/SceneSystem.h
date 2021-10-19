#pragma once

#include "Scene.h"
#include "Ecs.h"

#include <memory>

namespace nc
{
    class SceneSystem
    {
        public:
            SceneSystem();
            
            void QueueSceneChange(std::unique_ptr<Scene> swapScene);
            bool IsSceneChangeScheduled() const;
            void UnloadActiveScene();
            void DoSceneChange(registry_type* registry);

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            bool m_isSceneChangeScheduled;
    };
}