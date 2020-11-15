#include "SceneManagerImpl.h"

namespace nc::scene
{
    SceneManagerImpl::SceneManagerImpl()
        : m_activeScene{ nullptr },
          m_swapScene{ nullptr },
          m_isSceneChangeScheduled{ false }
    {
    }

    void SceneManagerImpl::QueueSceneChange(std::unique_ptr<Scene>&& swapScene)
    {
        m_swapScene = std::move(swapScene);
        m_isSceneChangeScheduled = true;
    }

    bool SceneManagerImpl::IsSceneChangeScheduled() const
    {
        return m_isSceneChangeScheduled;
    }

    void SceneManagerImpl::LoadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to load null scene");
        }
        m_activeScene->Load();
    }

    void SceneManagerImpl::UnloadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to unload null scene");
        }
        m_activeScene->Unload();
        m_activeScene = nullptr;
    }

    void SceneManagerImpl::DoSceneChange()
    {
        if(!m_swapScene)
        {
            throw std::runtime_error("Attempt to swap null scene");
        }
        m_activeScene = std::move(m_swapScene);
        m_swapScene = nullptr;
        m_isSceneChangeScheduled = false;
    }
}