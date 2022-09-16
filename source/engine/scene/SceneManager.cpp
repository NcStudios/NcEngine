#include "SceneManager.h"
#include "module/ModuleProvider.h"
#include "utility/Log.h"

namespace nc::scene
{
    SceneManager::SceneManager(std::function<void()> clearOnSceneChangeCallback)
        : m_activeScene{nullptr},
          m_swapScene{nullptr},
          m_clearCallback{std::move(clearOnSceneChangeCallback)}
    {
    }

    bool SceneManager::IsSceneChangeQueued() const noexcept
    {
        return static_cast<bool>(m_swapScene);
    }

    void SceneManager::QueueSceneChange(std::unique_ptr<Scene> swapScene) noexcept
    {
        NC_LOG_TRACE("Queueing scene change");
        m_swapScene = std::move(swapScene);
    }

    void SceneManager::DoSceneChange(Registry* registry, ModuleProvider modules)
    {
        NC_LOG_TRACE("Changing scene");
        if(!m_swapScene)
        {
            return;
        }

        if(m_activeScene)
        {
            NC_LOG_TRACE("Unloading active scene");
            m_activeScene->Unload();
            m_activeScene = nullptr;
        }

        m_clearCallback();
        m_activeScene = std::move(m_swapScene);
        m_swapScene = nullptr;
        NC_LOG_TRACE("Loading scene");
        m_activeScene->Load(registry, modules);
    }
}