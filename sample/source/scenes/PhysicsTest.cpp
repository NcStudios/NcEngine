#include "PhysicsTest.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"
#include "shared/spawner/Spawner.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/ui/ImGuiUtility.h"

namespace nc::sample
{
std::function<void(unsigned)> SpawnFunc = nullptr;
std::function<void(unsigned)> DestroyFunc = nullptr;

int SpawnCount = 1000;
int DestroyCount = 1000;

void Widget()
{
    ImGui::Text("Spawn Test");
    if(ImGui::BeginChild("Widget", {0,0}, true))
    {
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        ImGui::InputInt("##spawncount", &SpawnCount);
        SpawnCount = nc::Clamp(SpawnCount, 1, 20000);

        if(ImGui::Button("Spawn", {100, 20}))
            SpawnFunc(SpawnCount);

        ImGui::InputInt("##destroycount", &DestroyCount);
        DestroyCount = nc::Clamp(DestroyCount, 1, 20000);

        if(ImGui::Button("Destroy", {100, 20}))
            DestroyFunc(DestroyCount);

        ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
        ImGui::Text("-middle mouse button + drag to pan");
        ImGui::Text("-right mouse button + drag to look");
        ImGui::Text("-mouse wheel to zoom");
    }

    ImGui::EndChild();
}

auto BuildVehicle(ecs::Ecs world, physics::NcPhysics* ncPhysics) -> Entity
{
    const auto head = world.Emplace<Entity>({
        .tag = "Worm Head"
    });

    const auto segment1 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -0.9f},
        .scale = Vector3::Splat(0.8f),
        .tag = "Worm Segment"
    });

    const auto segment2 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -1.6f},
        .scale = Vector3::Splat(0.6f),
        .tag = "Worm Segment"
    });

    const auto segment3 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -2.1f},
        .scale = Vector3::Splat(0.4f),
        .tag = "Worm Segment"
    });

    world.Emplace<FixedLogic>(head, ForceBasedMovement);

    world.Emplace<graphics::ToonRenderer>(head, asset::CubeMesh, prefab::GreenToonMaterial);
    world.Emplace<graphics::ToonRenderer>(segment1, asset::CubeMesh, prefab::GreenToonMaterial);
    world.Emplace<graphics::ToonRenderer>(segment2, asset::CubeMesh, prefab::GreenToonMaterial);
    world.Emplace<graphics::ToonRenderer>(segment3, asset::CubeMesh, prefab::GreenToonMaterial);

    world.Emplace<physics::Collider>(head, physics::BoxProperties{}, false);
    world.Emplace<physics::Collider>(segment1, physics::BoxProperties{}, false);
    world.Emplace<physics::Collider>(segment2, physics::BoxProperties{}, false);
    world.Emplace<physics::Collider>(segment3, physics::BoxProperties{}, false);

    world.Emplace<physics::PhysicsBody>(head, physics::PhysicsProperties{.mass = 5.0f});
    world.Emplace<physics::PhysicsBody>(segment1, physics::PhysicsProperties{.mass = 3.0f});
    world.Emplace<physics::PhysicsBody>(segment2, physics::PhysicsProperties{.mass = 1.0f});
    world.Emplace<physics::PhysicsBody>(segment3, physics::PhysicsProperties{.mass = 0.2f});

    constexpr auto bias = 0.2f;
    constexpr auto softness = 0.1f;
    ncPhysics->AddJoint(head, segment1, Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
    ncPhysics->AddJoint(segment1, segment2, Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
    ncPhysics->AddJoint(segment2, segment3, Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);

    return head;
}

void BuildGround(ecs::Ecs world)
{
    constexpr auto extent = 150.0f;
    constexpr auto halfExtent = extent * 0.5f;

    auto ground = world.Emplace<Entity>({
        .position = Vector3{0.0f, -1.0f, 0.0f},
        .scale = Vector3{extent, 1.0f, extent},
        .tag = "Ground",
        .flags = Entity::Flags::Static
    });

    auto backWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto frontWall = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, -halfExtent},
        .scale = Vector3{extent, 3.0f, 2.0f},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto leftWall = world.Emplace<Entity>({
        .position = Vector3{-halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    auto rightWall = world.Emplace<Entity>({
        .position = Vector3{halfExtent, 0.0f, 0.0f},
        .scale = Vector3{2.0f, 3.0f, extent},
        .tag = "Wall",
        .flags = Entity::Flags::Static
    });

    world.Emplace<graphics::ToonRenderer>(ground, asset::CubeMesh, prefab::BlueToonMaterial);
    world.Emplace<graphics::ToonRenderer>(backWall, asset::CubeMesh, prefab::OrangeToonMaterial);
    world.Emplace<graphics::ToonRenderer>(frontWall, asset::CubeMesh, prefab::OrangeToonMaterial);
    world.Emplace<graphics::ToonRenderer>(leftWall, asset::CubeMesh, prefab::OrangeToonMaterial);
    world.Emplace<graphics::ToonRenderer>(rightWall, asset::CubeMesh, prefab::OrangeToonMaterial);

    world.Emplace<physics::Collider>(ground, physics::BoxProperties{});
    world.Emplace<physics::Collider>(backWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(frontWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(leftWall, physics::BoxProperties{});
    world.Emplace<physics::Collider>(rightWall, physics::BoxProperties{});
}

void BuildBridge(ecs::Ecs world, physics::NcPhysics* ncPhysics)
{
    // Platforms
    const auto platform1 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 5.0f, 40.0f},
        .scale = Vector3{10.0f, 1.0f, 10.0f},
        .tag = "Platform"
    });

    const auto platform2 = world.Emplace<Entity>({
        .position = Vector3{0.0f, 5.0f, 60.0f},
        .scale = Vector3{10.0f, 1.0f, 10.0f},
        .tag = "Platform"
    });

    world.Emplace<graphics::ToonRenderer>(platform1, asset::CubeMesh, prefab::TealToonMaterial);
    world.Emplace<graphics::ToonRenderer>(platform2, asset::CubeMesh, prefab::TealToonMaterial);
    world.Emplace<physics::Collider>(platform1, physics::BoxProperties{}, false);
    world.Emplace<physics::Collider>(platform2, physics::BoxProperties{}, false);
    world.Emplace<physics::PhysicsBody>(platform1, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});
    world.Emplace<physics::PhysicsBody>(platform2, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});

    // Bridge
    auto makePlank = [&](const Vector3& pos, const Vector3& scale)
    {
        const auto plank = world.Emplace<Entity>({.position = pos, .scale = scale, .tag = "Plank"});
        world.Emplace<graphics::ToonRenderer>(plank, asset::CubeMesh, prefab::OrangeToonMaterial);
        world.Emplace<physics::Collider>(plank, physics::BoxProperties{}, false);
        world.Emplace<physics::PhysicsBody>(plank, physics::PhysicsProperties{});
        return plank;
    };

    auto nextPos = Vector3{0.0f, 5.0f, 46.0f};
    constexpr auto offset = Vector3{0.0f, 0.0f, 2.0f};

    constexpr auto scale = Vector3{8.0f, 0.8f, 1.8f};

    const auto plank1 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank2 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank3 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank4 = makePlank(nextPos, scale);
    nextPos += offset;
    const auto plank5 = makePlank(nextPos, scale);


    float bias = 0.2f;
    float softness = 0.5f;

    ncPhysics->AddJoint(platform1, plank1, Vector3{-3.0f, 0.0f, 5.1f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(platform1, plank1, Vector3{3.0f, 0.0f, 5.1f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank1, plank2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank1, plank2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank2, plank3, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank2, plank3, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank3, plank4, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank3, plank4, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank4, plank5, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
    ncPhysics->AddJoint(plank4, plank5, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

    ncPhysics->AddJoint(plank5, platform2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -5.1f}, bias, softness);
    ncPhysics->AddJoint(plank5, platform2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -5.1f}, bias, softness);
}

////////////////////////////////

PhysicsTest::PhysicsTest(SampleUI* ui)
{
    ui->SetWidgetCallback(Widget);
}

void PhysicsTest::Load(Registry* registry, ModuleProvider modules)
{
    auto world = registry->GetEcs();
    auto* physics = modules.Get<physics::NcPhysics>();

    // Camera
    auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{1.20484f, 100.0f, -8.48875f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3{0.443f, 0.412f, 0.412f}, Vector3{0.4751f, 0.525f, 1.0f}, 600.0f);

    // Movable object
    BuildVehicle(world, physics);

    // Ground
    BuildGround(world);

    BuildBridge(world, physics);

    // Ramp
    auto ramp = registry->Add<Entity>({
        .position = Vector3{9.0f, 2.6f, 6.0f},
        .scale = Vector3::Splat(3.0f),
        .flags = Entity::Flags::Static
    });
    registry->Add<graphics::ToonRenderer>(ramp, "ramp.nca", prefab::RedToonMaterial);
    registry->Add<physics::ConcaveCollider>(ramp, "ramp.nca");

    // Hinge
    {
        auto anchor = registry->Add<Entity>({
            .position = Vector3{-10.0f, 3.25f, 10.0f},
            .scale = Vector3{3.0f, 0.5f, 0.5f}
        });
        registry->Add<graphics::ToonRenderer>(anchor, asset::CubeMesh, prefab::BlueToonMaterial);
        registry->Add<physics::Collider>(anchor, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(anchor, physics::PhysicsProperties{.isKinematic = true});

        auto panel = registry->Add<Entity>({
            .position = Vector3{-10.0f, 1.5f, 10.0f},
            .scale = Vector3{3.0f, 3.0f, 0.1f}
        });
        registry->Add<graphics::ToonRenderer>(panel, asset::CubeMesh, prefab::RedToonMaterial);
        registry->Add<physics::Collider>(panel, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(panel, physics::PhysicsProperties{});

        physics->AddJoint(anchor, panel, Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
        physics->AddJoint(anchor, panel, Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
    }

    // Balance platform
    {
        auto base = registry->Add<Entity>({
            .position = Vector3{10.0f, -0.75f, -10.0f},
            .scale = Vector3::Splat(2.0f)
        });
        registry->Add<graphics::ToonRenderer>(base, asset::SphereMesh, prefab::RedToonMaterial);
        registry->Add<physics::Collider>(base, physics::SphereProperties{}, false);
        registry->Add<physics::PhysicsBody>(base, physics::PhysicsProperties{.isKinematic = true}, Vector3::One(), Vector3::Zero());

        auto balancePlatform = registry->Add<Entity>({
            .position = Vector3{10.0f, -0.4f, -10.0f},
            .scale = Vector3{6.0f, 0.1f, 6.0f}
        });
        registry->Add<graphics::ToonRenderer>(balancePlatform, asset::CubeMesh, prefab::RedToonMaterial);
        registry->Add<physics::Collider>(balancePlatform, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(balancePlatform, physics::PhysicsProperties{.mass = 5.0f});

        physics->AddJoint(base, balancePlatform, Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
    }

    // Swinging bars
    {
        auto pole = world.Emplace<Entity>({
            .position = Vector3{-10.0f, 1.0f, -10.0f},
            .scale = Vector3{0.05f, 4.0f, 0.05f}
        });
        registry->Add<graphics::ToonRenderer>(pole, asset::CubeMesh, prefab::PurpleToonMaterial);
        registry->Add<physics::Collider>(pole, physics::BoxProperties{}, true);
        registry->Add<physics::PhysicsBody>(pole, physics::PhysicsProperties{.isKinematic = true});

        auto bar1 = world.Emplace<Entity>({
            .position = Vector3{-10.0f, -0.5f, -10.0f},
            .scale = Vector3{3.0f, 1.0f, 0.1f}
        });
        registry->Add<graphics::ToonRenderer>(bar1, asset::CubeMesh, prefab::YellowToonMaterial);
        registry->Add<physics::Collider>(bar1, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar1, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        auto bar2 = world.Emplace<Entity>({
            .position = Vector3{-10.0f, 1.0f, -10.0f},
            .scale = Vector3{3.0f, 1.0f, 0.1f}
        });
        registry->Add<graphics::ToonRenderer>(bar2, asset::CubeMesh, prefab::YellowToonMaterial);
        registry->Add<physics::Collider>(bar2, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar2, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        physics->AddJoint(pole, bar1, Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
        physics->AddJoint(pole, bar2, Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
    }

    // Fixed interval spawner for moving cubes
    SpawnBehavior dynamicCubeBehavior
    {
        .minPosition = Vector3{-70.0f, 20.0f, -70.0f},
        .maxPosition = Vector3{70.0f, 50.0f, 70.0f},
        .minRotation = Vector3::Zero(),
        .maxRotation = Vector3::Splat(std::numbers::pi_v<float> * 2.0f)
    };

    auto dynamicCubeExtension = [registry](Entity handle)
    {
        registry->Add<graphics::ToonRenderer>(handle, asset::CubeMesh, prefab::PurpleToonMaterial);
        registry->Add<physics::Collider>(handle, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(handle, physics::PhysicsProperties{.mass = 5.0f});
    };

    auto spawner = registry->Add<Entity>({});
    auto spawnerPtr = registry->Add<Spawner>(spawner, modules.Get<Random>(), dynamicCubeBehavior, dynamicCubeExtension);
    registry->Add<FrameLogic>(spawner, InvokeFreeComponent<Spawner>{});
    SpawnFunc = std::bind(&Spawner::StageSpawn, spawnerPtr, std::placeholders::_1);
    DestroyFunc = std::bind(&Spawner::StageDestroy, spawnerPtr, std::placeholders::_1);
}
}
