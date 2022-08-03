#pragma once

#include "scene/SceneModule.h"
#include "module/Job.h"

#include <functional>

namespace nc::scene
{
    auto BuildSceneModule(std::function<void()> clearOnSceneChangeCallback) -> std::unique_ptr<SceneModule>;

    class SceneModuleImpl final : public SceneModule
    {
        public:
            SceneModuleImpl(std::function<void()> clearOnSceneChangeCallback);

            bool IsSceneChangeScheduled() const override;
            void ChangeScene(std::unique_ptr<Scene> swapScene) override;
            void DoSceneSwap(Registry* registry, ModuleProvider modules) override;

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            std::function<void()> m_clearCallback;
    };
}