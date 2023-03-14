#include "JareTestScene.h"
#include "shared/FreeComponents.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/MeshRenderer.h"
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
    auto floorMaterial = graphics::Material{
        .baseColor = "floor\\BaseColor.nca",
        .normal    = "floor\\Normal.nca",
        .roughness = "floor\\Roughness.nca",
        .metallic  = "floor\\Roughness.nca"
    };

    auto blacktopMaterial = graphics::Material{
        .baseColor = "blacktop\\BaseColor.nca",
        .normal    = "blacktop\\Normal.nca",
        .roughness = "blacktop\\Roughness.nca",
        .metallic  = "blacktop\\Metallic.nca"
    };

    auto blueMaterial = graphics::Material{
        .baseColor = "spheres\\Blue\\BaseColor.nca",
        .normal    = "spheres\\Blue\\Normal.nca",
        .roughness = "spheres\\Blue\\Roughness.nca",
        .metallic  = "spheres\\Blue\\Metallic.nca"
    };

    auto grayMaterial = graphics::Material{
        .baseColor = "spheres\\Gray\\BaseColor.nca",
        .normal    = "spheres\\Gray\\Normal.nca",
        .roughness = "spheres\\Gray\\Roughness.nca",
        .metallic  = "spheres\\Gray\\Metallic.nca"
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox("DefaultSkybox.nca");

    //Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{2.5f, 4.0f, -1.4f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.4f, 0.4f, 0.8f), 88.0f);

    auto lvHandle2 = registry->Add<Entity>({.position = Vector3{-2.5f, 4.0f, -1.4f}, .tag = "Point Light 2"});
    registry->Add<graphics::PointLight>(lvHandle2, Vector3(0.1f, 0.1f, 0.1f), Vector3(0.4f, 0.8f, 0.4f), 88.0f);

    auto floor = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
        .scale = Vector3{30.0f, 30.0f, 1.0f},
        .tag = "Floor"
    });

    registry->Add<graphics::MeshRenderer>(floor, "plane.nca", floorMaterial);
    auto blueSphere = registry->Add<Entity>({
        .position = Vector3{0.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"
    });

    registry->Add<graphics::MeshRenderer>(blueSphere, "sphere.nca", blueMaterial);

    auto blackSphere = registry->Add<Entity>({
        .position = Vector3{3.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"
    });

    registry->Add<graphics::MeshRenderer>(blackSphere, "sphere.nca", grayMaterial);

    auto blackBox = registry->Add<Entity>({
        .position = Vector3{-3.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(0.2f, 0.7f, 0.4f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Box"
    });

    registry->Add<graphics::MeshRenderer>(blackBox, "cube.nca", blacktopMaterial);

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
