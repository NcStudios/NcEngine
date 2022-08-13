#pragma once

#include "scene/Scene.h"

#include <functional>

namespace nc::scene
{
    class SceneManager
    {
        public:
            SceneManager(std::function<void()> clearOnSceneChangeCallback);

            bool IsSceneChangeQueued() const noexcept;
            void QueueSceneChange(std::unique_ptr<Scene> swapScene) noexcept;
            void DoSceneChange(Registry* registry, ModuleProvider modules);

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            std::function<void()> m_clearCallback;
    };
}