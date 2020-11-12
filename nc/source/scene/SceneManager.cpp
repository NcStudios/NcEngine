#include "SceneManager.h"

namespace nc::scene
{
    SceneManager* SceneManager::m_instance = nullptr;

    SceneManager::SceneManager()
        : m_activeScene{ nullptr },
          m_swapScene{ nullptr },
          m_isSceneChangeScheduled{ false }
    {
        SceneManager::m_instance = this;
    }

    SceneManager::~SceneManager()
    {
        SceneManager::m_instance = nullptr;
    }

    void SceneManager::QueueSceneChange(std::unique_ptr<Scene> swapScene)
    {
        SceneManager::m_instance->QueueSceneChange_(std::move(swapScene));
    }

    void SceneManager::QueueSceneChange_(std::unique_ptr<Scene> swapScene)
    {
        m_swapScene = std::move(swapScene);
        m_isSceneChangeScheduled = true;
    }

    bool SceneManager::IsSceneChangeScheduled() const
    {
        return m_isSceneChangeScheduled;
    }

    void SceneManager::LoadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to load null scene");
        }
        m_activeScene->Load();
    }

    void SceneManager::UnloadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to unload null scene");
        }
        m_activeScene->Unload();
        m_activeScene = nullptr;
    }

    void SceneManager::DoSceneChange()
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