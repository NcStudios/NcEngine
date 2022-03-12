#include "SceneModuleImpl.h"

namespace nc::scene
{
    SceneModuleImpl::SceneModuleImpl(std::function<void()> clearOnSceneChangeCallback)
        : m_activeScene{nullptr},
          m_swapScene{nullptr},
          m_clearCallback{std::move(clearOnSceneChangeCallback)}
    {
    }

    void SceneModuleImpl::ChangeScene(std::unique_ptr<Scene> swapScene)
    {
        m_swapScene = std::move(swapScene);
    }

    void SceneModuleImpl::DoSceneSwap(NcEngine* engine)
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
        m_activeScene->Load(engine);
    }
}