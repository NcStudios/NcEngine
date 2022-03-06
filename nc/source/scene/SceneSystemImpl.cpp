#include "SceneSystemImpl.h"
#include "debug/Utils.h"

namespace nc::scene
{
    SceneSystemImpl::SceneSystemImpl()
        : m_activeScene{ nullptr },
          m_swapScene{ nullptr },
          m_isSceneChangeScheduled{ false }
    {
    }

    void SceneSystemImpl::ChangeScene(std::unique_ptr<Scene> swapScene)
    {
        m_swapScene = std::move(swapScene);
        m_isSceneChangeScheduled = true;
    }

    bool SceneSystemImpl::IsSceneChangeScheduled() const
    {
        return m_isSceneChangeScheduled;
    }

    void SceneSystemImpl::UnloadActiveScene()
    {
        if(!m_activeScene)
        {
            throw NcError("Attempt to unload null scene");
        }
        m_activeScene->Unload();
        m_activeScene = nullptr;
    }

    void SceneSystemImpl::DoSceneChange(nc_engine* engine)
    {
        if(!m_swapScene)
        {
            throw NcError("Attempt to swap null scene");
        }

        m_activeScene = std::move(m_swapScene);
        m_swapScene = nullptr;
        m_isSceneChangeScheduled = false;
        m_activeScene->Load(engine);
    }
}