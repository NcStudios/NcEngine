#include "CollisionEvents.h"
#include "NcEngine.h"
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
    void CollisionEvents::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();

        // Setup
        m_sceneHelper.Setup(engine, true, false, Widget);

        auto light = registry->Add<Entity>({.position = Vector3{0.1f, 4.2f, -5.8f}, .tag = "Point Light 1"});
        light.add<PointLight>(PointLightInfo{.pos = Vector3::Zero(),
                                             .ambient = Vector3{0.443f, 0.412f, 0.412f},
                                             .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
                                             .diffuseIntensity = 200.0});

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto* camera = cameraHandle.add<SceneNavigationCamera>(0.05f, 0.005f, 1.4f);
        cameraHandle.add<FrameLogic>(InvokeFreeComponent<SceneNavigationCamera>{});
        engine->MainCamera()->Set(camera);

        // Movable Objects
        auto spawner = registry->Add<Entity>({.tag = "Prefab Selector"});
        auto* prefabSelector = spawner.add<PrefabSelector>();
        spawner.add<FrameLogic>(InvokeFreeComponent<PrefabSelector>{});
        SelectPrefabCallback = std::bind_front(&PrefabSelector::Select, prefabSelector);

        // Dynamic Objects
        auto smallGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{3.5f, 0.0f, -1.0f}, .rotation = Quaternion::FromEulerAngles(0.0f, 0.785f, 0.0f), .tag = "Small Green Cube"});
        smallGreenCube.add<Collider>(BoxProperties{}, false);
        smallGreenCube.add<PhysicsBody>(PhysicsProperties{});

        auto smallGreenSphere = prefab::Create(registry, prefab::Resource::SphereGreen, {.position = Vector3{-3.5f, 0.0f, -1.0f}, .tag = "Small Green Sphere"});
        smallGreenSphere.add<Collider>(SphereProperties{.center = Vector3::Zero(), .radius = 0.5f}, false);
        smallGreenSphere.add<PhysicsBody>(PhysicsProperties{});

        auto bigGreenCube = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, 6.5f}, .scale = Vector3::Splat(2.0f), .tag = "Big Green Cube"});
        bigGreenCube.add<Collider>(BoxProperties{}, false);
        bigGreenCube.add<PhysicsBody>(PhysicsProperties{});

        auto greenCapsule = prefab::Create(registry, prefab::Resource::CapsuleGreen, {.position = Vector3{5.0f, 2.0f, 1.5f}, .tag = "Green Capsule"});
        greenCapsule.add<Collider>(CapsuleProperties{}, false);
        greenCapsule.add<PhysicsBody>(PhysicsProperties{});

        auto greenDisc = prefab::Create(registry, prefab::Resource::DiscGreen, {.position = Vector3{-6.0f, 0.0f, 1.5f}, .rotation = Quaternion::FromEulerAngles(1.57095f, 0.0f, 0.0f), .scale = Vector3::Splat(7.0f), .tag = "Green Disc"});
        greenDisc.add<Collider>(HullProperties{.assetPath = "coin.nca"}, false);
        greenDisc.add<PhysicsBody>(PhysicsProperties{});

        // Static Objects
        auto ground = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{0.0f, -1.5f, 0.0f}, .scale = Vector3{25.0f, 1.0f, 25.0f}, .tag = "Ground", .flags = Entity::Flags::Static});
        ground.add<Collider>(BoxProperties{}, false);

        auto bigRedSphere = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{-4.5f, 0.0f, 5.0f}, .scale = Vector3::Splat(3.0f), .tag = "Big Red Sphere", .flags = Entity::Flags::Static});
        bigRedSphere.add<Collider>(SphereProperties{}, false);

        nc::LoadCubeMapAsset("DefaultSkybox/DefaultSkybox.nca");
        engine->Environment()->SetSkybox("DefaultSkybox/DefaultSkybox.nca");
    }

    void CollisionEvents::Unload()
    {
        m_sceneHelper.TearDown();
    }
} //end namespace project