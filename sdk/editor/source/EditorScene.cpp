#include "EditorScene.h"
#include "ecs/component/SceneNavigationCamera.h"
#include "ecs/InvokeFreeComponent.h"
#include "framework/ProjectManager.h"

namespace nc::editor
{
    EditorScene::EditorScene(ProjectManager* projectManager)
        : m_projectManager{projectManager}
    {
    }

    void EditorScene::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();
        auto cameraEntity = registry->Add<Entity>({.position = Vector3{0, 0, -5}, .tag = EditorScene::EditorCameraTag});
        auto* camera = registry->Add<SceneNavigationCamera>(cameraEntity);
        registry->Add<FrameLogic>(cameraEntity, InvokeFreeComponent<SceneNavigationCamera>{});
        engine->Graphics()->SetCamera(camera);
        m_projectManager->ReadNextScene();
    }

    void EditorScene::Unload()
    {
    }
}