#include "VulkanScene.h"
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
        m_sceneHelper.Setup(registry, true, true, Widget);

        //Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.ambient = Vector3(1.0f, 0.7f, 1.0f),
                                                                           .diffuseColor = Vector3(0.8f, 0.6f, 1.0f),
                                                                           .diffuseIntensity = 2.0f
                                                                          });

        // Create the skeeball
        prefab::Create(registry,
                       prefab::Resource::Skeeball, 
                       {.position = Vector3{0.0f, 1.0f, 2.0f},
                        .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f),
                        .tag = "Skeeball"});

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