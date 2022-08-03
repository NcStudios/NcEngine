#include "EditorScene.h"
#include "framework/ProjectManager.h"

#include "ecs/component/SceneNavigationCamera.h"
#include "ecs/InvokeFreeComponent.h"
#include "graphics/GraphicsModule.h"

namespace nc::editor
{
    EditorScene::EditorScene(ProjectManager* projectManager)
        : m_projectManager{projectManager}
    {
    }

    void EditorScene::Load(Registry* registry, ModuleProvider modules)
    {
        auto cameraEntity = registry->Add<Entity>({.position = Vector3{0, 0, -5}, .tag = EditorScene::EditorCameraTag});
        auto* camera = registry->Add<SceneNavigationCamera>(cameraEntity);
        registry->Add<FrameLogic>(cameraEntity, InvokeFreeComponent<SceneNavigationCamera>{});
        modules.Get<GraphicsModule>()->SetCamera(camera);
        m_projectManager->ReadNextScene();
    }

    void EditorScene::Unload()
    {
    }
}