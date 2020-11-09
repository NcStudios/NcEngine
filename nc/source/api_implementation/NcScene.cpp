#include "NcScene.h"
#include "scene/SceneManager.h"
#include "NcDebug.h"

namespace nc::scene
{
    void NcChangeScene(std::unique_ptr<scene::Scene>&& scene)
    {
        V_LOG("Changing scene");
        SceneManager::QueueSceneChange(std::move(scene));
    }
}