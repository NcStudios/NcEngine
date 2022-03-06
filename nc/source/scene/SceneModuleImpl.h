#pragma once

#include "scene/SceneModule.h"

#include <functional>

/** @todo remove */
#include "task/TaskGraph.h"

namespace nc::scene
{
    class SceneModuleImpl final : public SceneModule
    {
        public:
            SceneModuleImpl(std::function<void()> clearOnSceneChangeCallback);

            void ChangeScene(std::unique_ptr<Scene> swapScene) override;
            void DoSceneSwap(NcEngine* engine) override;
            void Clear() noexcept override {}

            /** @todo fix */
            auto GetTasks() -> TaskGraph& override { static TaskGraph tg; return tg; }


        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            std::function<void()> m_clearCallback;
    };
}