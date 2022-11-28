#include "EditorScene.h"
#include "framework/ProjectManager.h"

#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

namespace nc::editor
{
    EditorScene::EditorScene(ProjectManager* projectManager)
        : m_projectManager{projectManager}
    {
    }

    void EditorScene::Load(Registry* registry, ModuleProvider modules)
    {
        auto cameraEntity = registry->Add<Entity>({.position = Vector3{0, 0, -5}, .tag = EditorScene::EditorCameraTag});
        auto* camera = registry->Add<graphics::SceneNavigationCamera>(cameraEntity);
        registry->Add<FrameLogic>(cameraEntity, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
        modules.Get<graphics::NcGraphics>()->SetCamera(camera);
        m_projectManager->ReadNextScene();
    }

    void EditorScene::Unload()
    {
    }
}