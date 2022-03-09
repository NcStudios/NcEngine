#pragma once

#include "scene/SceneModule.h"
#include "task/Job.h"

#include <functional>

namespace nc::scene
{
    class SceneModuleImpl final : public SceneModule
    {
        public:
            SceneModuleImpl(std::function<void()> clearOnSceneChangeCallback);

            void ChangeScene(std::unique_ptr<Scene> swapScene) override;
            void DoSceneSwap(NcEngine* engine) override;
            auto BuildWorkload() -> std::vector<Job> override { return {}; }
            void Clear() noexcept override {}

        private:
            std::unique_ptr<Scene> m_activeScene;
            std::unique_ptr<Scene> m_swapScene;
            std::function<void()> m_clearCallback;
    };
}