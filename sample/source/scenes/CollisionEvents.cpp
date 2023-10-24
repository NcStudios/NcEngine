#include "CollisionEvents.h"
#include "shared/Prefabs.h"
#include "shared/FreeComponents.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"

#include <functional>

namespace
{
using namespace nc::physics;

std::function<void(ColliderType)> SelectPrefabCallback = nullptr;
int PrefabTypeSelection = static_cast<int>(ColliderType::Capsule);

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

        for(auto v : {ColliderType::Box, ColliderType::Capsule, ColliderType::Sphere, ColliderType::Hull})
        {
            ImGui::SameLine();
            ImGui::RadioButton(nc::physics::ToString(v).data(), &PrefabTypeSelection, static_cast<int>(v));
        }

        if(ImGui::Button("Spawn"))
        {
            SelectPrefabCallback(static_cast<ColliderType>(PrefabTypeSelection));
        }

    } ImGui::EndChild();
}
} // anonymous namespace

namespace nc::sample
{
CollisionEvents::CollisionEvents(SampleUI* ui)
{
    ui->SetWidgetCallback(::Widget);
}

void CollisionEvents::Load(Registry* registry, ModuleProvider modules)
{
    auto lvHandle = registry->Add<Entity>({.position = Vector3{0.1f, 4.2f, -5.8f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3{0.443f, 0.412f, 0.412f}, Vector3{0.4751f, 0.525f, 1.0f}, 200.0f);

    // Camera
    auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
    auto* camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

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

    modules.Get<graphics::NcGraphics>()->SetSkybox(asset::DefaultCubeMap);
}
} // namespace nc::sample
