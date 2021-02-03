#include "SceneSystem.h"
#include "Scene.h"
#include "DebugUtils.h"

namespace
{
    nc::scene::SceneSystem* g_instance = nullptr;
}

namespace nc::scene
{
    /* Api Function Implementation */
    void Change(std::unique_ptr<scene::Scene>&& scene)
    {
        V_LOG("Changing scene");
        IF_THROW(!g_instance, "scene::Change - No SceneSystem instance set");
        g_instance->QueueSceneChange(std::move(scene));
    }

    /* SceneSystem */
    SceneSystem::SceneSystem()
        : m_activeScene{ nullptr },
          m_swapScene{ nullptr },
          m_isSceneChangeScheduled{ false }
    {
        g_instance = this;
    }

    void SceneSystem::QueueSceneChange(std::unique_ptr<Scene>&& swapScene)
    {
        m_swapScene = std::move(swapScene);
        m_isSceneChangeScheduled = true;
    }

    bool SceneSystem::IsSceneChangeScheduled() const
    {
        return m_isSceneChangeScheduled;
    }

    void SceneSystem::LoadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to load null scene");
        }
        m_activeScene->Load();
    }

    void SceneSystem::UnloadActiveScene()
    {
        if(!m_activeScene)
        {
            throw std::runtime_error("Attempt to unload null scene");
        }
        m_activeScene->Unload();
        m_activeScene = nullptr;
    }

    void SceneSystem::DoSceneChange()
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