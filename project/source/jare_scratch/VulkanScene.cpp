#include "VulkanScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"
#include "graphics/vulkan/Material.h"
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
        const std::vector<std::string> meshPaths { defaultMeshesPath + "plane.nca",
                                                   defaultMeshesPath + "Cube.nca"
                                                  };
        nc::graphics::vulkan::LoadMeshes(meshPaths); 

        // Load all scene textures
        const std::string defaultTexturesPath = "project/Textures/";
        const std::vector<std::string> texturePaths { defaultTexturesPath + "Floor/BaseColor.png", 
                                                      defaultTexturesPath + "Floor/Normal.png", 
                                                      defaultTexturesPath + "Floor/Roughness.png", 
                                                      defaultTexturesPath + "Box/BaseColor.png",
                                                      defaultTexturesPath + "Box/Normal.png",
                                                      defaultTexturesPath + "Box/Roughness.png"
                                                    };
        nc::graphics::vulkan::LoadTextures(texturePaths); 

        auto boxMaterial = nc::graphics::vulkan::Material{};
        boxMaterial.baseColor = defaultTexturesPath + "Box/BaseColor.png";
        boxMaterial.normal = defaultTexturesPath + "Box/Normal.png";
        boxMaterial.roughness = defaultTexturesPath + "Box/Roughness.png";

        // Lights
        auto lvHandle = CreateEntity({.position = Vector3{0.0f, 5.0f, 0.0f}, .tag = "Point Light 1"});
        AddComponent<vulkan::PointLight>(lvHandle, vulkan::PointLightInfo{.specularColor = Vector4(0.5f, 0.75f, 0.5f, 0.0f)});
        auto lvHandle2 = CreateEntity({.position = Vector3{15.0f, 5.0f, 15.0f}, .tag = "Point Light 2"});
        AddComponent<vulkan::PointLight>(lvHandle2, vulkan::PointLightInfo{.specularColor = Vector4(0.5f, 0.5f, 0.75f, 0.0f)});
        auto lvHandle3 = CreateEntity({.position = Vector3{30.0f, 5.0, 0.0f}, .tag = "Point Light 3"});
        AddComponent<vulkan::PointLight>(lvHandle3, vulkan::PointLightInfo{.specularColor = Vector4(0.75f, 0.5f, 0.5f, 0.0f)});

        // Create the entity
        auto box = CreateEntity({.position = Vector3{0.0f, 1.0f, 2.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .tag = "Box"});
        AddComponent<vulkan::MeshRenderer>(box, meshPaths[1], boxMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        AddComponent<WasdController>(box, 2.0f);

        // Create the flooring
        GenerateFloor(meshPaths);

        // Camera
        auto cameraHandle = CreateEntity({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = AddComponent<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);
    }

    void VulkanScene::Unload()
    {
        
    }

    void VulkanScene::GenerateFloor(const std::vector<std::string>& meshPaths)
    {
        const std::string defaultTexturesPath = "project/Textures/";
        auto floorMaterial = nc::graphics::vulkan::Material{};
        floorMaterial.baseColor = defaultTexturesPath + "Floor/BaseColor.png";
        floorMaterial.normal = defaultTexturesPath + "Floor/Normal.png";
        floorMaterial.roughness = defaultTexturesPath + "Floor/Roughness.png";

        auto floor1 = CreateEntity({.position = Vector3{-10.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor1, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor2 = CreateEntity({.position = Vector3{-10.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor2, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor3 = CreateEntity({.position = Vector3{-10.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor3, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor4 = CreateEntity({.position = Vector3{-10.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor4, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor5 = CreateEntity({.position = Vector3{0.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor5, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor6 = CreateEntity({.position = Vector3{10.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor6, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor7 = CreateEntity({.position = Vector3{30.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor7, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::Wireframe);

        auto floor8 = CreateEntity({.position = Vector3{0.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor8, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor9 = CreateEntity({.position = Vector3{10.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor9, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor10 = CreateEntity({.position = Vector3{20.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor10, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor11 = CreateEntity({.position = Vector3{30.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor11, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor12 = CreateEntity({.position = Vector3{0.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor12, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor13 = CreateEntity({.position = Vector3{10.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor13, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor14 = CreateEntity({.position = Vector3{20.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor14, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor15 = CreateEntity({.position = Vector3{30.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor15, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor16 = CreateEntity({.position = Vector3{0.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor16, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor17 = CreateEntity({.position = Vector3{10.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor17, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor18 = CreateEntity({.position = Vector3{20.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor18, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor19 = CreateEntity({.position = Vector3{30.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor19, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
            
        auto floor20 = CreateEntity({.position = Vector3{20.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        AddComponent<vulkan::MeshRenderer>(floor20, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    }
}