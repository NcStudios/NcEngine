#include "JointsTest.h"
#include "shared/FreeComponents.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"

#include "ncengine/NcEngine.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/physics/NcPhysics.h"

namespace nc::sample
{
JointsTest::JointsTest(SampleUI* ui)
{
    ui->SetWidgetCallback(nullptr);
}

void JointsTest::Load(Registry* registry, ModuleProvider modules)
{
    auto* physics = modules.Get<physics::NcPhysics>();

    // Camera
    auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);

    // Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{1.20484f, 9.4f, -8.48875f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, graphics::PointLightInfo{.pos = Vector3::Zero(),
                                                                            .ambient = Vector3{0.443f, 0.412f, 0.412f},
                                                                            .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
                                                                            .diffuseIntensity = 600});

    // Movable object
    {
        auto head = prefab::Create(registry, prefab::Resource::CubeGreen, {.tag = "Movable Object"});
        registry->Add<physics::Collider>(head, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(head, physics::PhysicsProperties{.mass = 5.0f});
        registry->Add<FixedLogic>(head, ForceBasedMovement);

        auto segment1 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -0.9f}, .scale = Vector3::Splat(0.8f)});
        registry->Add<physics::Collider>(segment1, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(segment1, physics::PhysicsProperties{.mass = 3.0f});

        auto segment2 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -1.6f}, .scale = Vector3::Splat(0.6f)});
        registry->Add<physics::Collider>(segment2, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(segment2, physics::PhysicsProperties{.mass = 1.0f});

        auto segment3 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -2.1f}, .scale = Vector3::Splat(0.4f)});
        registry->Add<physics::Collider>(segment3, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(segment3, physics::PhysicsProperties{.mass = 0.2f});

        float bias = 0.2f;
        float softness = 0.1f;

        physics->AddJoint(head, segment1, Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
        physics->AddJoint(segment1, segment2, Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
        physics->AddJoint(segment2, segment3, Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);
    }

    // Ground
    auto platform1 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{0.0f, -1.0f, 0.0f}, .scale = Vector3{30.0f, 1.0f, 30.0f}, .tag = "Platform1"});
    registry->Add<physics::Collider>(platform1, physics::BoxProperties{}, false);
    registry->Add<physics::PhysicsBody>(platform1, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});

    auto platform2 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{8.0f, -1.0f, 30.0f}, .scale = Vector3{10.0f, 1.0f, 10.0f}, .tag = "Platform2"});
    registry->Add<physics::Collider>(platform2, physics::BoxProperties{}, false);
    registry->Add<physics::PhysicsBody>(platform2, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});

    // Ramp
    {
        auto ramp = prefab::Create(registry, prefab::Resource::RampRed, {.position = Vector3{9.0f, 2.6f, 6.0f}, .scale = Vector3::Splat(3.0f), .flags = Entity::Flags::Static});
        registry->Add<physics::ConcaveCollider>(ramp, "ramp.nca");
    }

    // Hinge
    {
        auto anchor = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 3.25f, 10.0f}, .scale = Vector3{3.0f, 0.5f, 0.5f}});
        registry->Add<physics::Collider>(anchor, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(anchor, physics::PhysicsProperties{.isKinematic = true});

        auto panel = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.5f, 10.0f}, .scale = Vector3{3.0f, 3.0f, 0.1f}});
        registry->Add<physics::Collider>(panel, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(panel, physics::PhysicsProperties{});

        physics->AddJoint(anchor, panel, Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
        physics->AddJoint(anchor, panel, Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
    }

    // Balance platform
    {
        auto base = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{10.0f, -0.75f, -10.0f}, .scale = Vector3::Splat(2.0f)});
        registry->Add<physics::Collider>(base, physics::SphereProperties{}, false);
        registry->Add<physics::PhysicsBody>(base, physics::PhysicsProperties{.isKinematic = true}, Vector3::One(), Vector3::Zero());

        auto balancePlatform = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{10.0f, -0.4f, -10.0f}, .scale = Vector3{6.0f, 0.1f, 6.0f}});
        registry->Add<physics::Collider>(balancePlatform, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(balancePlatform, physics::PhysicsProperties{.mass = 5.0f});

        physics->AddJoint(base, balancePlatform, Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
    }

    // Swinging bars
    {
        auto pole = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{0.05f, 4.0f, 0.05f}});
        registry->Add<physics::Collider>(pole, physics::BoxProperties{}, true);
        registry->Add<physics::PhysicsBody>(pole, physics::PhysicsProperties{.isKinematic = true});

        auto bar1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, -0.5f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
        registry->Add<physics::Collider>(bar1, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar1, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        auto bar2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
        registry->Add<physics::Collider>(bar2, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(bar2, physics::PhysicsProperties{}, Vector3::One(), Vector3::Up());

        physics->AddJoint(pole, bar1, Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
        physics->AddJoint(pole, bar2, Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
    }

    // Bridge
    {
        auto plank1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 16.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
        registry->Add<physics::Collider>(plank1, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(plank1, physics::PhysicsProperties{});

        auto plank2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 18.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
        registry->Add<physics::Collider>(plank2, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(plank2, physics::PhysicsProperties{});

        auto plank3 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 20.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
        registry->Add<physics::Collider>(plank3, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(plank3, physics::PhysicsProperties{});

        auto plank4 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 22.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
        registry->Add<physics::Collider>(plank4, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(plank4, physics::PhysicsProperties{});

        auto plank5 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 24.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
        registry->Add<physics::Collider>(plank5, physics::BoxProperties{}, false);
        registry->Add<physics::PhysicsBody>(plank5, physics::PhysicsProperties{});

        float bias = 0.2f;
        float softness = 0.5f;

        physics->AddJoint(platform1, plank1, Vector3{5.0f, 0.0f, 15.1f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
        physics->AddJoint(platform1, plank1, Vector3{11.0f, 0.0f, 15.1f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

        physics->AddJoint(plank1, plank2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
        physics->AddJoint(plank1, plank2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

        physics->AddJoint(plank2, plank3, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
        physics->AddJoint(plank2, plank3, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

        physics->AddJoint(plank3, plank4, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
        physics->AddJoint(plank3, plank4, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

        physics->AddJoint(plank4, plank5, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
        physics->AddJoint(plank4, plank5, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

        physics->AddJoint(plank5, platform2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -5.1f}, bias, softness);
        physics->AddJoint(plank5, platform2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -5.1f}, bias, softness);
    }
}
}
