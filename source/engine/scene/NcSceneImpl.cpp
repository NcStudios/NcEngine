#include "NcSceneImpl.h"
#include "ncengine/module/ModuleProvider.h"
#include "ncengine/utility/Log.h"

#include "ncutility/NcError.h"

namespace nc
{
auto BuildSceneModule() -> std::unique_ptr<NcScene>
{
    NC_LOG_TRACE("Building NcScene module");
    return std::make_unique<SceneManager>();
}

auto SceneManager::Queue(std::unique_ptr<Scene> scene) noexcept -> size_t
{
    NC_LOG_TRACE("Queueing scene");
    m_sceneQueue.push_back(std::move(scene));
    return m_sceneQueue.size() - 1;
}

void SceneManager::DequeueScene(size_t queuePosition)
{
    NC_LOG_TRACE("Dequeueing scene");
    NC_ASSERT(queuePosition < m_sceneQueue.size(), "Invalid scene queuePosition");
    m_sceneQueue.erase(m_sceneQueue.begin() + queuePosition);
}

auto SceneManager::GetNumberOfScenesInQueue() const noexcept -> size_t
{
    return m_sceneQueue.size();
}

void SceneManager::ScheduleTransition() noexcept
{
    NC_LOG_TRACE("Scheduling scene transition");
    m_transitionScheduled = true;
}

auto SceneManager::IsTransitionScheduled() const noexcept -> bool
{
    return m_transitionScheduled;
}

auto SceneManager::UnloadActiveScene() -> bool
{
    NC_LOG_TRACE("Unloading active scene");
    if(!m_activeScene)
    {
        NC_LOG_WARNING("No scene loaded - ignoring scene unload request.");
        return false;
    }

    m_activeScene->Unload();
    m_activeScene = nullptr;
    return true;
}

auto SceneManager::LoadQueuedScene(Registry* registry, ModuleRegistry& modules)-> bool
{
    NC_LOG_TRACE("Starting scene change");
    m_transitionScheduled = false;
    NC_ASSERT(!m_activeScene, "Attempt to change scenes with an active scene loaded.");
    if(m_sceneQueue.empty())
    {
        NC_LOG_WARNING("No scene queued - ignoring scene change request.");
        return false;
    }

    NC_LOG_TRACE("Notifying OnBeforeSceneLoad()");
    for (auto& module : modules.GetAllModules())
    {
        module->OnBeforeSceneLoad();
    }

    NC_LOG_TRACE("Loading scene");
    m_activeScene = std::move(m_sceneQueue.front());
    m_sceneQueue.erase(m_sceneQueue.begin());
    m_activeScene->Load(registry, ModuleProvider{&modules});
    return true;
}
} // namespace nc::scene
