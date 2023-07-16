#include "EditorScene.h"

#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

namespace
{
auto MakeEditorCamera(nc::Registry* registry) -> nc::graphics::SceneNavigationCamera*
{
    constexpr auto cameraPos = nc::Vector3{0.0f, 0.0f, -5.0f};
    auto entity = registry->Add<nc::Entity>({.position = cameraPos, .tag = nc::editor::EditorScene::EditorCameraTag});
    auto* camera = registry->Add<nc::graphics::SceneNavigationCamera>(entity);
    registry->Add<nc::FrameLogic>(entity, nc::InvokeFreeComponent<nc::graphics::SceneNavigationCamera>{});
    return camera;
}
} // anonymous namespace

namespace nc::editor
{
EditorScene::EditorScene()
{
}

void EditorScene::Load(Registry* registry, ModuleProvider modules)
{
    auto camera = ::MakeEditorCamera(registry);
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);
}

void EditorScene::Unload()
{
}
} // namespace nc::editor
