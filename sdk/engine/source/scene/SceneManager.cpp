#include "SceneManager.h"
#include "module/ModuleProvider.h"

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
        m_swapScene = std::move(swapScene);
    }

    void SceneManager::DoSceneChange(Registry* registry, ModuleProvider modules)
    {
        if(!m_swapScene)
        {
            return;
        }

        if(m_activeScene)
        {
            m_activeScene->Unload();
            m_activeScene = nullptr;
        }

        m_clearCallback();
        m_activeScene = std::move(m_swapScene);
        m_swapScene = nullptr;
        m_activeScene->Load(registry, modules);
    }
}