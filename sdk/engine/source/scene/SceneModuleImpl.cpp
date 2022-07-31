#include "SceneModuleImpl.h"
#include "module/ModuleProvider.h"

namespace nc::scene
{
    auto BuildSceneModule(std::function<void()> clearOnSceneChangeCallback) -> std::unique_ptr<SceneModule>
    {
        return std::make_unique<SceneModuleImpl>(std::move(clearOnSceneChangeCallback));
    }

    SceneModuleImpl::SceneModuleImpl(std::function<void()> clearOnSceneChangeCallback)
        : m_activeScene{nullptr},
          m_swapScene{nullptr},
          m_clearCallback{std::move(clearOnSceneChangeCallback)}
    {
    }

    bool SceneModuleImpl::IsSceneChangeScheduled() const
    {
        return static_cast<bool>(m_swapScene);
    }

    void SceneModuleImpl::ChangeScene(std::unique_ptr<Scene> swapScene)
    {
        m_swapScene = std::move(swapScene);
    }

    void SceneModuleImpl::DoSceneSwap(Registry* registry, ModuleProvider modules)
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