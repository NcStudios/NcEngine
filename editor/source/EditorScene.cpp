#include "EditorScene.h"
#include "SceneNavigationCamera.h"
#include "MainCamera.h"
#include "ecs/InvokeFreeComponent.h"
#include "framework/ProjectManager.h"

namespace nc::editor
{
    EditorScene::EditorScene(ProjectManager* projectManager)
        : m_projectManager{projectManager}
    {
    }

    void EditorScene::Load(nc_engine* engine)
    {
        auto* registry = engine->Registry();
        auto cameraEntity = registry->Add<Entity>({.position = Vector3{0, 0, -5}, .tag = EditorScene::EditorCameraTag});
        auto* camera = registry->Add<SceneNavigationCamera>(cameraEntity, 0.25f, 0.005f, 1.4f);
        registry->Add<FrameLogic>(cameraEntity, InvokeFreeComponent<SceneNavigationCamera>{});
        engine->Graphics()->set_camera(camera);
        m_projectManager->ReadNextScene();
    }

    void EditorScene::Unload()
    {
    }
}