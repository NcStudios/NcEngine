#pragma once

#include "scene/Scene.h"

namespace nc::scene
{
    class SceneManager
    {
        public:
            SceneManager();

            auto IsSceneChangeQueued() const noexcept -> bool;
            void QueueSceneChange(std::unique_ptr<Scene> swapScene) noexcept;
            auto UnloadActiveScene() -> bool;
            auto LoadQueuedScene(Registry* registry, ModuleProvider modules) -> bool;

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
    };
}
