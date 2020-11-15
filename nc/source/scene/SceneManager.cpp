#include "SceneManager.h"
#include "DebugUtils.h"

namespace nc::scene
{
    std::function<void(std::unique_ptr<scene::Scene>&&)> SceneManager::ChangeScene_ = nullptr;

    void SceneManager::ChangeScene(std::unique_ptr<scene::Scene>&& scene)
    {
        V_LOG("Changing scene");
        IF_THROW(!SceneManager::ChangeScene_, "SceneManager::ChangeScene_ is not bound");
        SceneManager::ChangeScene_(std::move(scene));
    }
}