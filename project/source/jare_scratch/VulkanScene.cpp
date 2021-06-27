#include "VulkanScene.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/TechniqueType.h"
#include "shared/SceneNavigationCamera.h"
#include "collision_events/WasdController.h"
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
    void VulkanScene::Load(registry_type* registry)
    {
        m_sceneHelper.Setup(registry, true, true, Widget);

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
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 5.0f, 0.0f}, .tag = "Point Light 1"});
        registry->Add<vulkan::PointLight>(lvHandle, vulkan::PointLightInfo{.specularColor = Vector4(0.5f, 0.75f, 0.5f, 0.0f)});
        auto lvHandle2 = registry->Add<Entity>({.position = Vector3{15.0f, 5.0f, 15.0f}, .tag = "Point Light 2"});
        registry->Add<vulkan::PointLight>(lvHandle2, vulkan::PointLightInfo{.specularColor = Vector4(0.5f, 0.5f, 0.75f, 0.0f)});
        auto lvHandle3 = registry->Add<Entity>({.position = Vector3{30.0f, 5.0, 0.0f}, .tag = "Point Light 3"});
        registry->Add<vulkan::PointLight>(lvHandle3, vulkan::PointLightInfo{.specularColor = Vector4(0.75f, 0.5f, 0.5f, 0.0f)});
        registry->Add<WasdController>(lvHandle3, registry, 2.0f);

        // Create the entity
        auto box = registry->Add<Entity>({.position = Vector3{0.0f, 1.0f, 2.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .tag = "Box"});
        registry->Add<vulkan::MeshRenderer>(box, meshPaths[1], boxMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        // Create the flooring
        GenerateFloor(registry, meshPaths);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);
    }

    void VulkanScene::Unload()
    {
        m_sceneHelper.TearDown();
    }

    void VulkanScene::GenerateFloor(registry_type* registry, const std::vector<std::string>& meshPaths)
    {
        const std::string defaultTexturesPath = "project/Textures/";
        auto floorMaterial = nc::graphics::vulkan::Material{};
        floorMaterial.baseColor = defaultTexturesPath + "Floor/BaseColor.png";
        floorMaterial.normal = defaultTexturesPath + "Floor/Normal.png";
        floorMaterial.roughness = defaultTexturesPath + "Floor/Roughness.png";

        auto floor1 = registry->Add<Entity>({.position = Vector3{-10.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor1, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor2 = registry->Add<Entity>({.position = Vector3{-10.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor2, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor3 = registry->Add<Entity>({.position = Vector3{-10.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor3, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor4 = registry->Add<Entity>({.position = Vector3{-10.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor4, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor5 = registry->Add<Entity>({.position = Vector3{0.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor5, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor6 = registry->Add<Entity>({.position = Vector3{10.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor6, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor7 = registry->Add<Entity>({.position = Vector3{30.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor7, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::Wireframe);

        auto floor8 = registry->Add<Entity>({.position = Vector3{0.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor8, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor9 = registry->Add<Entity>({.position = Vector3{10.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor9, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor10 = registry->Add<Entity>({.position = Vector3{20.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor10, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor11 = registry->Add<Entity>({.position = Vector3{30.0f, 0.0f, 0.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor11, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor12 = registry->Add<Entity>({.position = Vector3{0.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor12, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor13 = registry->Add<Entity>({.position = Vector3{10.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor13, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor14 = registry->Add<Entity>({.position = Vector3{20.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor14, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor15 = registry->Add<Entity>({.position = Vector3{30.0f, 0.0f, 10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor15, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor16 = registry->Add<Entity>({.position = Vector3{0.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor16, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor17 = registry->Add<Entity>({.position = Vector3{10.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor17, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
        
        auto floor18 = registry->Add<Entity>({.position = Vector3{20.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor18, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

        auto floor19 = registry->Add<Entity>({.position = Vector3{30.0f, 0.0f, 20.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor19, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
            
        auto floor20 = registry->Add<Entity>({.position = Vector3{20.0f, 0.0f, -10.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .scale = Vector3{5.0f, 5.0f, 1.0f}, .tag = "Floor"});
        registry->Add<vulkan::MeshRenderer>(floor20, meshPaths[0], floorMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);
    }
}