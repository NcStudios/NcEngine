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
    LoadTextureAsset("line\\Hatch.nca");
    LoadTextureAsset("line\\Hatch2.nca");
    LoadTextureAsset("line\\Hatch3.nca");
    LoadTextureAsset("line\\Noise.nca");
    LoadTextureAsset("line\\Dots.nca");
    LoadTextureAsset("tree\\Normal.nca");
    
    auto floorMaterial = graphics::ToonMaterial{
        .baseColor = "floor\\BaseColor.nca",
        .overlay    = "floor\\Normal.nca",
        .lightShading = "line\\Hatch2.nca",
        .heavyShading  = "line\\Hatch3.nca"
    };

    auto blacktopMaterial = graphics::ToonMaterial{
        .baseColor = "blacktop\\BaseColor.nca",
        .overlay    = "blacktop\\Normal.nca",
        .lightShading = "line\\Hatch2.nca",
        .heavyShading  = "line\\Hatch3.nca"
    };

    auto blueMaterial = graphics::ToonMaterial{
        .baseColor = "spheres\\Blue\\BaseColor.nca",
        .overlay    = "spheres\\Blue\\Normal.nca",
        .lightShading = "line\\Hatch2.nca",
        .heavyShading  = "line\\Hatch3.nca"
    };

    auto treeMaterial = graphics::ToonMaterial{
        .baseColor = "tree\\BaseColor.nca",
        .overlay    = "spheres\\Gray\\Normal.nca",
        .lightShading = "tree\\Normal.nca",
        .heavyShading  = "line\\Hatch3.nca"
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox("DefaultSkybox.nca");
    LoadMeshAsset("tree.nca");
    LoadMeshAsset("hill.nca");

    //Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{2.5f, 4.0f, -1.4f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.85f, 0.64f, 0.125f), 88.0f);

    auto lvHandle2 = registry->Add<Entity>({.position = Vector3{-2.5f, 4.0f, -1.4f}, .tag = "Point Light 2"});
    registry->Add<graphics::PointLight>(lvHandle2, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.4f, 0.8f, 0.4f), 88.0f);

    auto hill = registry->Add<Entity>({
        .position = Vector3{0.0f, -2.7f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
        .scale = Vector3{30.0f, 30.0f, 10.0f},
        .tag = "Hill"
    });
    registry->Add<graphics::ToonRenderer>(hill, "hill.nca", blueMaterial);

    auto blueSphere = registry->Add<Entity>({
        .position = Vector3{0.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"
    });

    registry->Add<graphics::ToonRenderer>(blueSphere, "sphere.nca", blueMaterial);

    auto blackSphere = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "Tree"
    });

    registry->Add<graphics::ToonRenderer>(blackSphere, "tree.nca", treeMaterial);

    auto blackBox = registry->Add<Entity>({
        .position = Vector3{-3.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(0.2f, 0.7f, 0.4f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Box"
    });

    registry->Add<graphics::ToonRenderer>(blackBox, "cube.nca", blacktopMaterial);

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
