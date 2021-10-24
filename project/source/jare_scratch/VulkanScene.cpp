#include "VulkanScene.h"
#include "Assets.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "shared/SceneNavigationCamera.h"
#include "collision_events/WasdController.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"

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
    void VulkanScene::Load(registry_type* registry)
    {
        m_sceneHelper.Setup(registry, true, false, Widget);

        const std::string defaultTexturesPath = "project/Textures/";

        const std::vector<std::string> texturePaths
        {
            defaultTexturesPath + "Floor/BaseColor.png",
            defaultTexturesPath + "Floor/Normal.png",
            defaultTexturesPath + "Floor/Roughness.png",
            defaultTexturesPath + "Snail/BaseColor.png",
            defaultTexturesPath + "Snail/Normal.png",
            defaultTexturesPath + "Snail/Roughness.png"
        };

        nc::LoadTextureAssets(texturePaths);

        auto floorMaterial =  graphics::Material
        { 
            .baseColor = defaultTexturesPath + "Floor/BaseColor.png",
            .normal    = defaultTexturesPath + "Floor/Normal.png",
            .roughness = defaultTexturesPath + "Floor/Roughness.png" 
        };

        auto snailMaterial =  graphics::Material
        { 
            .baseColor = defaultTexturesPath + "Snail/BaseColor.png",
            .normal    = defaultTexturesPath + "Snail/Normal.png",
            .roughness = defaultTexturesPath + "Snail/Roughness.png" 
        };

        const std::vector<std::string> sceneMeshes = std::vector<std::string>
        { 
            "project/assets/mesh/plane.nca",
            "project/assets/mesh/snail.nca"
        };

        nc::LoadMeshAssets(sceneMeshes);

        //Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, -0.5f, -4.0f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(0.1f, 0.1f, 0.1f),
                                                            .diffuseColor = Vector3(0.8f, 0.8f, 0.8f),
                                                            .diffuseIntensity = 2.0f
                                                            });

        auto lvHandle2 = registry->Add<Entity>({.position = Vector3{1.1f, 4.1f, -8.0f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle2, PointLightInfo{.ambient = Vector3(0.1f, 0.1f, 0.1f),
                                                            .diffuseColor = Vector3(0.8f, 0.8f, 0.8f),
                                                            .diffuseIntensity = 2.0f
                                                            });

        auto floor = registry->Add<Entity>(
            {.position = Vector3{0.0f, 0.0f, 0.0f},
             .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
             .scale = Vector3{30.0f, 30.0f, 1.0f},
             .tag = "Floor"});

        registry->Add<MeshRenderer>(floor, "project/assets/mesh/plane.nca", floorMaterial, nc::graphics::TechniqueType::PhongAndUi);

        auto snail = registry->Add<Entity>(
            {.position = Vector3{0.0f, 0.0f, 2.0f},
             .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 0.0f),
             .scale = Vector3{1.0f, 1.0f, 1.0f},
             .tag = "Snail"});

        registry->Add<MeshRenderer>(snail, "project/assets/mesh/snail.nca", snailMaterial, nc::graphics::TechniqueType::PhongAndUi);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{-0.0f, 4.0f, -6.4f}, .rotation = Quaternion::FromEulerAngles(0.4f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);
    }

    void VulkanScene::Unload()
    {
        m_sceneHelper.TearDown();
    }
}