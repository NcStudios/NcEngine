#include "JareTestScene.h"
#include "shared/FreeComponents.h"

#include "NcEngine.h"
#include "asset/Assets.h"
#include "ecs/InvokeFreeComponent.h"
#include "graphics/GraphicsModule.h"
#include "graphics/MeshRenderer.h"
#include "graphics/SceneNavigationCamera.h"
#include "imgui/imgui.h"

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
        const std::vector<std::string> texturePaths
        {
            "floor/BaseColor.png",
            "floor/Normal.png",
            "floor/Roughness.png",
            "blacktop/BaseColor.png",
            "blacktop/Normal.png",
            "blacktop/Roughness.png",
            "blacktop/Metallic.png",
            "spheres/Blue/BaseColor.png",
            "spheres/Blue/Normal.png",
            "spheres/Blue/Roughness.png",
            "spheres/Blue/Metallic.png",
            "spheres/Gray/BaseColor.png",
            "spheres/Gray/Normal.png",
            "spheres/Gray/Roughness.png",
            "spheres/Gray/Metallic.png",
        };

        nc::LoadTextureAssets(texturePaths);

        auto floorMaterial = graphics::Material
        { 
            .baseColor = "floor/BaseColor.png",
            .normal    = "floor/Normal.png",
            .roughness = "floor/Roughness.png",
            .metallic  = "floor/Roughness.png"
        };

        auto blacktopMaterial = graphics::Material
        { 
            .baseColor = "blacktop/BaseColor.png",
            .normal    = "blacktop/Normal.png",
            .roughness = "blacktop/Roughness.png",
            .metallic  = "blacktop/Metallic.png"
        };

        auto blueMaterial = graphics::Material
        { 
            .baseColor = "spheres/Blue/BaseColor.png",
            .normal    = "spheres/Blue/Normal.png",
            .roughness = "spheres/Blue/Roughness.png",
            .metallic  = "spheres/Blue/Metallic.png"
        };

        auto grayMaterial = graphics::Material
        { 
            .baseColor = "spheres/Gray/BaseColor.png",
            .normal    = "spheres/Gray/Normal.png",
            .roughness = "spheres/Gray/Roughness.png",
            .metallic  = "spheres/Gray/Metallic.png"
        };

        const std::vector<std::string> sceneMeshes = std::vector<std::string>
        { 
            "plane.nca",
            "sphere.nca",
            "cube.nca",
        };

        nc::LoadMeshAssets(sceneMeshes);
        nc::LoadCubeMapAsset("DefaultSkybox/DefaultSkybox.nca");
        modules.Get<graphics::GraphicsModule>()->SetSkybox("DefaultSkybox/DefaultSkybox.nca");

        //Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{-1.1f, 4.0f, -1.4f}, .tag = "Point Light 1"});
        registry->Add<graphics::PointLight>(lvHandle, graphics::PointLightInfo{.ambient = Vector3(0.4f, 0.4f, 0.4f),
                                                                               .diffuseColor = Vector3(0.8f, 0.8f, 0.8f),
                                                                               .diffuseIntensity = 88.0f});

        auto floor = registry->Add<Entity>(
        {.position = Vector3{0.0f, 0.0f, 0.0f},
         .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
         .scale = Vector3{30.0f, 30.0f, 1.0f},
         .tag = "Floor"});

        registry->Add<graphics::MeshRenderer>(floor, "plane.nca", floorMaterial, graphics::TechniqueType::PhongAndUi);

        auto blueSphere = registry->Add<Entity>(
        {.position = Vector3{0.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"});

        registry->Add<graphics::MeshRenderer>(blueSphere, "sphere.nca", blueMaterial, graphics::TechniqueType::PhongAndUi);

        auto blackSphere = registry->Add<Entity>(
        {.position = Vector3{3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Sphere"});

        registry->Add<graphics::MeshRenderer>(blackSphere, "sphere.nca", grayMaterial, graphics::TechniqueType::PhongAndUi);

        auto blackBox = registry->Add<Entity>(
        {.position = Vector3{-3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(0.2f, 0.7f, 0.4f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Box"});

        registry->Add<graphics::MeshRenderer>(blackBox, "cube.nca", blacktopMaterial, graphics::TechniqueType::PhongAndUi);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{-0.0f, 4.0f, -6.4f}, .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
        registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
        modules.Get<graphics::GraphicsModule>()->SetCamera(camera);
    }
}