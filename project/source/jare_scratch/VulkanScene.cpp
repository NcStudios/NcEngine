#include "VulkanScene.h"
#include "Assets.h"
#include "collision_events/WasdController.h"
#include "ecs/component/MeshRenderer.h"
#include "imgui/imgui.h"
#include "NcEngine.h"
#include "shared/SceneNavigationCamera.h"

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
    void VulkanScene::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();
        
        m_sceneHelper.Setup(engine, true, false, Widget);

        const std::vector<std::string> texturePaths
        {
            "Floor/BaseColor.png",
            "Floor/Normal.png",
            "Floor/Roughness.png",
            "Blacktop/BaseColor.png",
            "Blacktop/Normal.png",
            "Blacktop/Roughness.png",
            "Blacktop/Metallic.png",
            "Spheres/Blue/BaseColor.png",
            "Spheres/Blue/Normal.png",
            "Spheres/Blue/Roughness.png",
            "Spheres/Blue/Metallic.png",
            "Spheres/Gray/BaseColor.png",
            "Spheres/Gray/Normal.png",
            "Spheres/Gray/Roughness.png",
            "Spheres/Gray/Metallic.png",
        };

        nc::LoadTextureAssets(texturePaths);

        auto floorMaterial = Material
        { 
            .baseColor = "Floor/BaseColor.png",
            .normal    = "Floor/Normal.png",
            .roughness = "Floor/Roughness.png",
            .metallic  = "Floor/Roughness.png"
        };

        auto blacktopMaterial = Material
        { 
            .baseColor = "Blacktop/BaseColor.png",
            .normal    = "Blacktop/Normal.png",
            .roughness = "Blacktop/Roughness.png",
            .metallic  = "Blacktop/Metallic.png"
        };

        auto blueMaterial = Material
        { 
            .baseColor = "Spheres/Blue/BaseColor.png",
            .normal    = "Spheres/Blue/Normal.png",
            .roughness = "Spheres/Blue/Roughness.png",
            .metallic  = "Spheres/Blue/Metallic.png"
        };

        auto grayMaterial = Material
        { 
            .baseColor = "Spheres/Gray/BaseColor.png",
            .normal    = "Spheres/Gray/Normal.png",
            .roughness = "Spheres/Gray/Roughness.png",
            .metallic  = "Spheres/Gray/Metallic.png"
        };

        const std::vector<std::string> sceneMeshes = std::vector<std::string>
        { 
            "plane.nca",
            "sphere.nca",
            "cube.nca",
        };

        nc::LoadMeshAssets(sceneMeshes);
        nc::LoadCubeMapAsset("Humus02/Humus02.nca");
        engine->Environment()->SetSkybox("Humus02/Humus02.nca");
        
        //Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{-1.1f, 4.0f, -1.4f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(0.4f, 0.4f, 0.4f),
                                                           .diffuseColor = Vector3(0.8f, 0.8f, 0.8f),
                                                           .diffuseIntensity = 88.0f});

        auto floor = registry->Add<Entity>(
        {.position = Vector3{0.0f, 0.0f, 0.0f},
         .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
         .scale = Vector3{30.0f, 30.0f, 1.0f},
         .tag = "Floor"});

        registry->Add<MeshRenderer>(floor, "plane.nca", floorMaterial, TechniqueType::PhongAndUi);

        auto blueSphere = registry->Add<Entity>(
        {.position = Vector3{0.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"});

        registry->Add<MeshRenderer>(blueSphere, "sphere.nca", blueMaterial, TechniqueType::PhongAndUi);

        auto blackSphere = registry->Add<Entity>(
        {.position = Vector3{3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Sphere"});

        registry->Add<MeshRenderer>(blackSphere, "sphere.nca", grayMaterial, TechniqueType::PhongAndUi);

        auto blackBox = registry->Add<Entity>(
        {.position = Vector3{-3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(0.2, 0.7f, 0.4f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Box"});

        registry->Add<MeshRenderer>(blackBox, "cube.nca", blacktopMaterial, TechniqueType::PhongAndUi);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{-0.0f, 4.0f, -6.4f}, .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        engine->MainCamera()->Set(camera);
    }

    void VulkanScene::Unload()
    {
        m_sceneHelper.TearDown();
    }
}