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

        const std::string defaultTexturesPath = "project/Textures/";

        const std::vector<std::string> texturePaths
        {
            defaultTexturesPath + "Floor/BaseColor.png",
            defaultTexturesPath + "Floor/Normal.png",
            defaultTexturesPath + "Floor/Roughness.png",
            defaultTexturesPath + "Blacktop/BaseColor.png",
            defaultTexturesPath + "Blacktop/Normal.png",
            defaultTexturesPath + "Blacktop/Roughness.png",
            defaultTexturesPath + "Blacktop/Metallic.png",
            defaultTexturesPath + "Spheres/Blue/BaseColor.png",
            defaultTexturesPath + "Spheres/Blue/Normal.png",
            defaultTexturesPath + "Spheres/Blue/Roughness.png",
            defaultTexturesPath + "Spheres/Blue/Metallic.png",
            defaultTexturesPath + "Spheres/Gray/BaseColor.png",
            defaultTexturesPath + "Spheres/Gray/Normal.png",
            defaultTexturesPath + "Spheres/Gray/Roughness.png",
            defaultTexturesPath + "Spheres/Gray/Metallic.png",
        };

        nc::LoadTextureAssets(texturePaths);

        auto floorMaterial = Material
        { 
            .baseColor = defaultTexturesPath + "Floor/BaseColor.png",
            .normal    = defaultTexturesPath + "Floor/Normal.png",
            .roughness = defaultTexturesPath + "Floor/Roughness.png",
            .metallic  = defaultTexturesPath + "Floor/Roughness.png"
        };

        auto blacktopMaterial = Material
        { 
            .baseColor = defaultTexturesPath + "Blacktop/BaseColor.png",
            .normal    = defaultTexturesPath + "Blacktop/Normal.png",
            .roughness = defaultTexturesPath + "Blacktop/Roughness.png",
            .metallic  = defaultTexturesPath + "Blacktop/Metallic.png"
        };

        auto blueMaterial = Material
        { 
            .baseColor = defaultTexturesPath + "Spheres/Blue/BaseColor.png",
            .normal    = defaultTexturesPath + "Spheres/Blue/Normal.png",
            .roughness = defaultTexturesPath + "Spheres/Blue/Roughness.png",
            .metallic  = defaultTexturesPath + "Spheres/Blue/Metallic.png"
        };

        auto grayMaterial = Material
        { 
            .baseColor = defaultTexturesPath + "Spheres/Gray/BaseColor.png",
            .normal    = defaultTexturesPath + "Spheres/Gray/Normal.png",
            .roughness = defaultTexturesPath + "Spheres/Gray/Roughness.png",
            .metallic  = defaultTexturesPath + "Spheres/Gray/Metallic.png"
        };

        const std::vector<std::string> sceneMeshes = std::vector<std::string>
        { 
            "project/assets/mesh/plane.nca",
            "project/assets/mesh/sphere.nca",
            "project/assets/mesh/cube.nca"
        };

        nc::LoadMeshAssets(sceneMeshes);

        auto skybox = CubeMapFaces
        {
            .usage = CubeMapUsage::Skybox,
            .uid = "Humus01Skybox",
            .frontPath = defaultTexturesPath + "CubeMaps/Humus01/Front.jpg",
            .backPath  = defaultTexturesPath + "CubeMaps/Humus01/Back.jpg",
            .upPath    = defaultTexturesPath + "CubeMaps/Humus01/Up.jpg",
            .downPath  = defaultTexturesPath + "CubeMaps/Humus01/Down.jpg",
            .rightPath = defaultTexturesPath + "CubeMaps/Humus01/Right.jpg",
            .leftPath  = defaultTexturesPath + "CubeMaps/Humus01/Left.jpg"
        };

        nc::LoadCubeMapAsset(skybox);

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

        registry->Add<MeshRenderer>(floor, "project/assets/mesh/plane.nca", floorMaterial, TechniqueType::PhongAndUi);

        auto blueSphere = registry->Add<Entity>(
        {.position = Vector3{0.0f, 1.0f, 2.0f},
        .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
        .scale = Vector3{2.0f, 2.0f,2.0f},
        .tag = "Sphere"});

        registry->Add<MeshRenderer>(blueSphere, "project/assets/mesh/sphere.nca", blueMaterial, TechniqueType::PhongAndUi);

        auto blackSphere = registry->Add<Entity>(
        {.position = Vector3{3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(-1.5708f, 0.0f, 0.0f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Sphere"});

        registry->Add<MeshRenderer>(blackSphere, "project/assets/mesh/cube.nca", grayMaterial, TechniqueType::PhongAndUi);

        auto blackBox = registry->Add<Entity>(
        {.position = Vector3{-3.0f, 1.0f, 2.0f},
         .rotation = Quaternion::FromEulerAngles(0.2, 0.7f, 0.4f),
         .scale = Vector3{2.0f, 2.0f,2.0f},
         .tag = "Box"});

        registry->Add<MeshRenderer>(blackBox, "project/assets/mesh/cube.nca", blacktopMaterial, TechniqueType::PhongAndUi);

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