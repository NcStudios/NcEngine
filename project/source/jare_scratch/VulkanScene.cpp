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
        const std::vector<std::string> meshPaths { defaultMeshesPath + "skeeball.nca" };
        nc::graphics::vulkan::LoadMeshes(meshPaths); 

        // Load all scene textures
        const std::string defaultTexturesPath = "project/Textures/";
        const std::vector<std::string> texturePaths { "nc/resources/texture/DefaultBaseColor.png",
                                                      defaultTexturesPath + "SolidColor/Yellow.png"
                                                    };
        nc::graphics::vulkan::LoadTextures(texturePaths); 

        auto blueMaterial = nc::graphics::vulkan::Material{};
        blueMaterial.baseColor = "nc/resources/texture/DefaultBaseColor.png";
        blueMaterial.normal = defaultTexturesPath + "SolidColor/Yellow.png";
        blueMaterial.roughness = defaultTexturesPath + "SolidColor/Yellow.png";

        // Lights
        // auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        // registry->Add<vulkan::PointLight>(lvHandle, vulkan::PointLightInfo{.ambient = Vector4(1.0f, 0.7f, 1.0f, 0.0f),
        //                                                                    .diffuseColor = Vector4(0.8f, 0.6f, 1.0f, 0.0f),
        //                                                                    .specularColor = Vector4(1.0f, 0.9f, 1.0f, 0.0f),
        //                                                                    .diffuseIntensity = 2.0f
        //                                                                   });

        // Create the skeeball
        auto skeeballCourt = registry->Add<Entity>({.position = Vector3{0.0f, 1.0f, 2.0f}, .rotation = Quaternion::FromEulerAngles(1.5708f, 0.0f, 1.5708f), .tag = "Box"});
        registry->Add<vulkan::MeshRenderer>(skeeballCourt, meshPaths[0], blueMaterial, nc::graphics::vulkan::TechniqueType::PhongAndUi);

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