#include "SandboxScene.h"

#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

namespace nc::ui::editor
{
void SandboxScene::Load(Registry* registry, ModuleProvider modules)
{
    // will want point light
    // everything should have 'Internal'/'NoSerialize' flags
    // potentially nest everything under one parent

    const auto bucket = registry->Add<Entity>({
        .tag = "[Internal]"
    });

    const auto cameraHandle = registry->Add<Entity>({
        .position = Vector3{0.0f, 6.1f, -6.5f},
        .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
        .parent = bucket,
        .tag = "Scene Camera"
    });

    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);
}
} // namespace nc::ui::editor
