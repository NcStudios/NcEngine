#include "VulkanScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"
#include "graphics/vulkan/PhongMaterial.h"
#include "graphics/vulkan/TechniqueType.h"
#include "shared/SceneNavigationCamera.h"
#include "shared/WasdController.h"
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
    void VulkanScene::Load()
    {
        m_sceneHelper.Setup(true, true, Widget);

        // Load all scene meshes
        const std::string defaultMeshesPath = "project/assets/mesh/";
        const std::vector<std::string> meshPaths { defaultMeshesPath + "beeper.nca", 
                                                   defaultMeshesPath + "table.nca", 
                                                   defaultMeshesPath + "planet.nca"
                                                  };
        nc::graphics::vulkan::LoadMeshes(meshPaths); 

        // Load all scene textures
        const std::string defaultTexturesPath = "project/Textures/";
        const std::vector<std::string> texturePaths { defaultTexturesPath + "Beeper/BaseColor.png", 
                                                      defaultTexturesPath + "Beeper/Normal.png", 
                                                      defaultTexturesPath + "Beeper/Roughness.png", 
                                                      defaultTexturesPath + "Planets/Jupiter/BaseColor.png",
                                                      defaultTexturesPath + "Planets/Jupiter/Normal.png",
                                                      defaultTexturesPath + "Planets/Jupiter/Roughness.png",
                                                     };
        nc::graphics::vulkan::LoadTextures(texturePaths); 

        // Create the entity
        auto beeMaterial = nc::graphics::vulkan::PhongMaterial{};
        beeMaterial.baseColor = defaultTexturesPath + "Beeper/BaseColor.png";
        beeMaterial.normal = defaultTexturesPath + "Beeper/Normal.png";
        beeMaterial.roughness = defaultTexturesPath + "Beeper/Roughness.png";

        auto jupiterMaterial = nc::graphics::vulkan::PhongMaterial{};
        jupiterMaterial.baseColor = defaultTexturesPath + "Planets/Jupiter/BaseColor.png";
        jupiterMaterial.normal = defaultTexturesPath + "Planets/Jupiter/Normal.png";
        jupiterMaterial.roughness = defaultTexturesPath + "Planets/Jupiter/Roughness.png";

        auto handle = CreateEntity();
        AddComponent<vulkan::MeshRenderer>(handle, meshPaths[0], beeMaterial, nc::graphics::vulkan::TechniqueType::Simple);
        auto transform = GetComponent<Transform>(handle);
        transform->SetPosition(Vector3{2.0f, 1.0f, 1.0f});

        // Create the entity
        auto planet = CreateEntity();
        AddComponent<vulkan::MeshRenderer>(planet, meshPaths[2], jupiterMaterial, nc::graphics::vulkan::TechniqueType::Simple);

        // Create the entity
        auto handle2 = CreateEntity();
        AddComponent<vulkan::MeshRenderer>(handle2, meshPaths[1], beeMaterial, nc::graphics::vulkan::TechniqueType::Simple);
        AddComponent<WasdController>(handle2, 2.0f);

        // Create the entity
        auto handle4 = CreateEntity();
        AddComponent<vulkan::MeshRenderer>(handle4, meshPaths[0], beeMaterial, nc::graphics::vulkan::TechniqueType::Simple);
        auto transform2 = GetComponent<Transform>(handle4);
        transform2->SetPosition(Vector3{3.0f, 1.0f, 1.0f});
        
        // Camera
        auto cameraHandle = CreateEntity({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = AddComponent<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);
    }

    void VulkanScene::Unload()
    {
        
    }
}