#include "JareTestScene.h"
#include "shared/FreeComponents.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
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
    LoadTextureAsset("ogre\\BaseColor.nca");
    LoadMeshAsset("ogre.nca");
    LoadSkeletalAnimationAsset("ogre\\idle.nca");

    auto floorMaterial = graphics::ToonMaterial{
        .baseColor = asset::DefaultMetallic,
        .overlay   = asset::DefaultBaseColor,
        .hatching  = asset::DefaultBaseColor,
        .hatchingTiling  = 1
    };

    auto treeMaterial = graphics::ToonMaterial{
        .baseColor = "tree\\BaseColor.nca",
        .overlay   = asset::DefaultBaseColor,
        .hatching  = "line\\Hatch3.nca",
        .hatchingTiling  = 8
    };

    auto ogreMaterial = graphics::ToonMaterial{
        .baseColor = "ogre\\BaseColor.nca",
        .overlay   = asset::DefaultBaseColor,
        .hatching  = "line\\Hatch3.nca",
        .hatchingTiling  = 8
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox(asset::DefaultSkyboxCubeMap);

    auto ogre = registry->Add<Entity>({
            .position = Vector3{-5.0f, 0.0f, 12.0f},
            .rotation = Quaternion::FromEulerAngles(0.0f, 1.0f, 0.0f),
            .scale = Vector3{3.0f, 3.0f, 3.0f},
            .tag = "ogre"
    });
    registry->Add<graphics::ToonRenderer>(ogre, "ogre.nca", ogreMaterial);
    registry->Add<graphics::SkeletalAnimator>(ogre, "ogre.nca", "ogre\\idle.nca");

    //Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{2.5f, 4.0f, -1.4f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.85f, 0.64f, 0.125f), 88.0f);

    auto floor = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
        .scale = Vector3{30.0f, 30.0f, 1.0f},
        .tag = "Floor"
    });
    registry->Add<graphics::ToonRenderer>(floor, asset::PlaneMesh, floorMaterial);

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