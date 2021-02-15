#include "SpawnTest.h"
#include "MainCamera.h"
#include "imgui/imgui.h"
#include "KillBox.h"
#include "shared/spawner/FixedIntervalSpawner.h"
#include "shared/SceneNavigationCamera.h"

namespace
{
    void Widget()
    {
        ImGui::Text("Spawn Test");
        if(ImGui::BeginChild("Widget", {0,0}, true))
        {
            ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
            ImGui::Text("-middle mouse button + drag to pan");
            ImGui::Text("-right mouse button + drag to look");
            ImGui::Text("-mouse wheel to zoom");
        } ImGui::EndChild();
    }
}

namespace nc::sample
{
    void SpawnTest::Load()
    {
        // Setup
        m_sceneHelper.Setup(true, false, Widget);

        // Camera
        auto cameraHandle = CreateEntity({.position = Vector3{0.0f, -9.0f, -100.0f}, .tag = "SceneNavigationCamera"});
        auto camera = AddComponent<SceneNavigationCamera>(cameraHandle);
        camera::SetMainCamera(camera);

        // Point Light
        PointLight::Properties lightProperties
        {
            .ambient = Vector3::One(),
            .diffuseIntensity = 4.5f,
            .attConst = 1.0f,
            .attLin = 0.0001f
        };
        auto lvHandle = CreateEntity({.tag = "Point Light"});
        auto pointLight = AddComponent<PointLight>(lvHandle);
        pointLight->Set(lightProperties);

        // Collider that destroys anything leaving its bounded area
        auto killBox = CreateEntity({.scale = Vector3::Splat(60.0f), .tag = "KillBox"});
        AddComponent<Collider>(killBox, ColliderType::Box, Vector3::One());
        AddComponent<KillBox>(killBox);

        // Spawner for stationary cubes
        SpawnBehavior staticCubeBehavior
        {
            .positionRandomRange = Vector3::Splat(55.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
        };

        auto staticCubeExtension = [](EntityHandle handle)
        {
            GetComponent<Transform>(handle)->SetScale(Vector3::Splat(6.0f));
            AddComponent<Collider>(handle, ColliderType::Box, Vector3::One());
        };

        auto staticCubeSpawnerHandle = CreateEntity({.tag = "Static Cube Spawner"});
        auto staticCubeSpawner = AddComponent<Spawner>(staticCubeSpawnerHandle, prefab::Resource::CubeBlue, staticCubeBehavior, staticCubeExtension);
        staticCubeSpawner->Spawn(5);
        staticCubeSpawner->SetPrefab(prefab::Resource::CubeRed);
        staticCubeSpawner->Spawn(5);

        // Fixed interval spawner for moving cubes
        SpawnBehavior dynamicCubeBehavior
        {
            .positionRandomRange = Vector3::Splat(15.0f),
            .rotationRandomRange = Vector3::Splat(math::Pi / 2.0f),
            .velocityRandomRange = Vector3::Splat(10.0f),
            .rotationAxisRandomRange = Vector3::One(),
            .thetaRandomRange = 2.0f
        };

        auto dynamicCubeExtension = [](EntityHandle handle)
        {
            AddComponent<Collider>(handle, ColliderType::Box, Vector3::One());
        };
        
        auto dynamicCubeSpawnerHandle = CreateEntity({.tag = "Dynamic Cube Spawner"});
        AddComponent<FixedIntervalSpawner>(dynamicCubeSpawnerHandle, prefab::Resource::CubeGreen, dynamicCubeBehavior, 0.2f, dynamicCubeExtension);
    }

    void SpawnTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
} // end namespace project