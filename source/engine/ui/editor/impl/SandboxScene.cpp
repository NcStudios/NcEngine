#include "SandboxScene.h"

#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/serialize/SceneSerialization.h"

#include <fstream>

namespace nc::ui::editor
{
void SandboxScene::Load(Registry* registry, ModuleProvider modules)
{
    if (!m_fragmentPath.empty())
    {
        auto file = std::ifstream{m_fragmentPath, std::ios::binary};
        NC_ASSERT(file.is_open(), fmt::format("Failed to open scene fragment file '{}'", m_fragmentPath));
        LoadSceneFragment(file, registry->GetEcs(), *modules.Get<asset::NcAsset>());
        return;
    }

    // const auto bucket = registry->Add<Entity>({
    //     .tag = "[Internal]",
    //     .flags = Entity::Flags::Internal | Entity::Flags::NoSerialize
    // });

    // const auto cameraHandle = registry->Add<Entity>({
    //     .position = Vector3{0.0f, 6.1f, -6.5f},
    //     .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f),
    //     .parent = m_bucket,
    //     .tag = "Scene Camera",
    //     .flags = Entity::Flags::Internal | Entity::Flags::NoSerialize
    // });

    // auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    // registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    // modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // auto pointLight = registry->Add<Entity>({
    //     .position = Vector3{5.0f, 10.0f, -10.0f},
    //     .parent = m_bucket,
    //     .tag = "Point Light",
    //     .flags = Entity::Flags::Internal | Entity::Flags::NoSerialize
    // });

    // registry->Add<graphics::PointLight>(pointLight);
}
} // namespace nc::ui::editor
