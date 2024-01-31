#pragma once

#include "scene/NcScene.h"

#include <vector>

namespace nc
{
class SceneManager : public NcScene
{
    public:
        auto Queue(std::unique_ptr<Scene> scene) noexcept -> size_t override;
        void DequeueScene(size_t queuePosition) override;
        auto GetNumberOfScenesInQueue() const noexcept -> size_t override;
        void ScheduleTransition() noexcept override;
        auto IsTransitionScheduled() const noexcept -> bool override;

        // auto IsSceneChangeQueued() const noexcept -> bool override;
        // void QueueSceneChange(std::unique_ptr<Scene> swapScene) noexcept override;
        auto UnloadActiveScene() -> bool override;
        auto LoadQueuedScene(Registry* registry, ModuleRegistry& modules) -> bool override;

    private:
        std::unique_ptr<Scene> m_activeScene;
        std::vector<std::unique_ptr<Scene>> m_sceneQueue;
        bool m_transitionScheduled = false;
        // std::unique_ptr<Scene> m_swapScene;
};
} // namespace nc
