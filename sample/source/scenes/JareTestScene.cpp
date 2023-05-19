#include "JareTestScene.h"
#include "shared/FreeComponents.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

#include <string>

namespace
{
void Widget()
{
    ImGui::Text("Vulkan");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ImGui::Text("Vulkan scratch scene.");
    } ImGui::EndChild();
}
}

namespace nc::sample
{
JareTestScene::JareTestScene(SampleUI* ui)
{
    ui->SetWidgetCallback(::Widget);
}

void JareTestScene::Load(Registry* registry, ModuleProvider modules)
{
    LoadTextureAsset("tree\\BaseColor.nca");
    LoadTextureAsset("line\\Hatch3.nca");
    
    auto floorMaterial = graphics::ToonMaterial{
        .baseColor = "DefaultMetallic.nca",
        .overlay   = "DefaultBaseColor.nca",
        .hatching  = "DefaultBaseColor.nca",
        .hatchingTiling  = 1
    };

    auto treeMaterial = graphics::ToonMaterial{
        .baseColor = "tree\\BaseColor.nca",
        .overlay   = "DefaultBaseColor.nca",
        .hatching  = "line\\Hatch3.nca",
        .hatchingTiling  = 8
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox("DefaultSkybox.nca");
    LoadMeshAsset("tree.nca");

    //Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{2.5f, 4.0f, -1.4f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.85f, 0.64f, 0.125f), 88.0f);

    auto floor = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
        .scale = Vector3{30.0f, 30.0f, 1.0f},
        .tag = "Floor"
    });
    registry->Add<graphics::ToonRenderer>(floor, "plane.nca", floorMaterial);

    auto tree = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "Tree"
    });
    registry->Add<graphics::ToonRenderer>(tree, "tree.nca", treeMaterial);

    // Camera
    auto cameraHandle = registry->Add<Entity>({
        .position = Vector3{-0.0f, 4.0f, -6.4f},
        .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f),
        .tag = "Main Camera"
    });

    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);
}
}
