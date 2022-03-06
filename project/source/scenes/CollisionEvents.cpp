#include "CollisionEvents.h"
#include "nc_engine.h"
#include "ecs/InvokeFreeComponent.h"
#include "imgui/imgui.h"
#include "shared/Prefabs.h"
#include "shared/FreeComponents.h"

#include <functional>

namespace
{
    std::function<void(nc::ColliderType)> SelectPrefabCallback = nullptr;
    int PrefabTypeSelection = static_cast<int>(nc::ColliderType::Capsule);

    void Widget()
    {
        ImGui::Text("Collision Events");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Text("Check the log for collision events.");
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("-move camera by edge panning");
            ImGui::Text("-zoom with mouse wheel");
            ImGui::Text("-WASD to control the blue cube");

            ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
        
            for(auto v : {nc::ColliderType::Box, nc::ColliderType::Capsule, nc::ColliderType::Sphere, nc::ColliderType::Hull})
            {
                ImGui::SameLine();
                ImGui::RadioButton(nc::ToCString(v), &PrefabTypeSelection, static_cast<int>(v));
            }

            if(ImGui::Button("Spawn"))
            {
                SelectPrefabCallback(static_cast<nc::ColliderType>(PrefabTypeSelection));
            }

        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void CollisionEvents::Load(nc_engine* engine)
    {
        auto* registry = engine->Registry();

        // Setup
        m_sceneHelper.Setup(engine, true, false, Widget);

        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.1f, 4.2f, -5.8f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.pos = Vector3::Zero(),
                                                           .ambient = Vector3{0.443f, 0.412f, 0.412f},
                                                           .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
                                                           .diffuseIntensity = 200.0});

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto* camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<SceneNavigationCamera>{});
        engine->Graphics()->set_camera(camera);

        // Movable Objects
        auto objectSpawner = registry->Add<Entity>({.tag = "Prefab Selector"});
        auto* prefabSelector = registry->Add<PrefabSelector>(objectSpawner);
        registry->Add<FrameLogic>(objectSpawner, InvokeFreeComponent<PrefabSelector>{});
        SelectPrefabCallback = std::bind(&PrefabSelector::Select, prefabSelector, std::placeholders::_1);

        // Dynamic Objects
        auto smallGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{3.5f, 0.0f, -1.0f}, .rotation = Quaternion::FromEulerAngles(0.0f, 0.785f, 0.0f), .tag = "Small Green Cube"});
        registry->Add<Collider>(smallGreenCube, BoxProperties{}, false);
        registry->Add<PhysicsBody>(smallGreenCube, PhysicsProperties{});

        auto smallGreenSphere = prefab::Create(registry, prefab::Resource::SphereGreen, {.position = Vector3{-3.5f, 0.0f, -1.0f}, .tag = "Small Green Sphere"});
        registry->Add<Collider>(smallGreenSphere, SphereProperties{.center = Vector3::Zero(), .radius = 0.5f}, false);
        registry->Add<PhysicsBody>(smallGreenSphere, PhysicsProperties{});

        auto bigGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, 6.5f}, .scale = Vector3::Splat(2.0f), .tag = "Big Green Cube"});
        registry->Add<Collider>(bigGreenCube, BoxProperties{}, false);
        registry->Add<PhysicsBody>(bigGreenCube, PhysicsProperties{});

        auto greenCapsule = prefab::Create(registry, prefab::Resource::CapsuleGreen, {.position = Vector3{5.0f, 2.0f, 1.5f}, .tag = "Green Capsule"});
        registry->Add<Collider>(greenCapsule, CapsuleProperties{}, false);
        registry->Add<PhysicsBody>(greenCapsule, PhysicsProperties{});

        auto greenDisc = prefab::Create(registry, prefab::Resource::DiscGreen, {.position = Vector3{-6.0f, 0.0f, 1.5f}, .rotation = Quaternion::FromEulerAngles(1.57095f, 0.0f, 0.0f), .scale = Vector3::Splat(7.0f), .tag = "Green Disc"});
        registry->Add<Collider>(greenDisc, HullProperties{.assetPath = "coin.nca"}, false);
        registry->Add<PhysicsBody>(greenDisc, PhysicsProperties{});

        // Static Objects
        auto ground = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{0.0f, -1.5f, 0.0f}, .scale = Vector3{25.0f, 1.0f, 25.0f}, .tag = "Ground", .flags = Entity::Flags::Static});
        registry->Add<Collider>(ground, BoxProperties{}, false);

        auto bigRedSphere = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{-4.5f, 0.0f, 5.0f}, .scale = Vector3::Splat(3.0f), .tag = "Big Red Sphere", .flags = Entity::Flags::Static});
        registry->Add<Collider>(bigRedSphere, SphereProperties{}, false);

        nc::LoadCubeMapAsset("DefaultSkybox/DefaultSkybox.nca");
        engine->Graphics()->set_skybox("DefaultSkybox/DefaultSkybox.nca");
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project