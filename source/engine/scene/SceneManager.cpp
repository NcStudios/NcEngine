#include "SceneManager.h"
#include "module/ModuleProvider.h"
#include "utility/Log.h"

#include "ncutility/NcError.h"

namespace nc::scene
{
SceneManager::SceneManager()
    : m_activeScene{nullptr},
      m_swapScene{nullptr}
{
}

auto SceneManager::IsSceneChangeQueued() const noexcept -> bool
{
    return static_cast<bool>(m_swapScene);
}

void SceneManager::QueueSceneChange(std::unique_ptr<Scene> swapScene) noexcept
{
    NC_LOG_TRACE("Queueing scene change");
    m_swapScene = std::move(swapScene);
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

auto SceneManager::LoadQueuedScene(Registry* registry, ModuleProvider modules)-> bool
{
    NC_LOG_TRACE("Changing scene");
    NC_ASSERT(!m_activeScene, "Attempt to change scenes with an active scene loaded.");
    if(!m_swapScene)
    {
        NC_LOG_WARNING("No scene queued - ignoring scene change request.");
        return false;
    }

    m_activeScene = std::move(m_swapScene);
    m_swapScene = nullptr;
    NC_LOG_TRACE("Loading scene");
    m_activeScene->Load(registry, modules);
    return true;
}
} // namespace nc::scene
