#include "JointsTest.h"
#include "NcEngine.h"
#include "ecs/InvokeFreeComponent.h"
#include "physics/PhysicsSystem.h"
#include "shared/FreeComponents.h"
#include "shared/GameLogic.h"
#include "shared/Prefabs.h"

namespace nc::sample
{
    void JointsTest::Load(NcEngine* engine)
    {
        auto* registry = engine->Registry();
        auto* physics = engine->Physics();

        m_sceneHelper.Setup(engine, false, false, nullptr);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = cameraHandle.add<SceneNavigationCamera>(0.05f, 0.005f, 1.4f);
        cameraHandle.add<FrameLogic>(InvokeFreeComponent<SceneNavigationCamera>{});
        engine->MainCamera()->Set(camera);

        // Lights
        auto lightHandle = registry->Add<Entity>({.position = Vector3{1.20484f, 9.4f, -8.48875f}, .tag = "Point Light"});
        lightHandle.add<PointLight>(PointLightInfo{.pos = Vector3::Zero(),
                                                   .ambient = Vector3{0.443f, 0.412f, 0.412f},
                                                   .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
                                                   .diffuseIntensity = 600});

        // Movable object
        {
            auto head = prefab::Create(registry, prefab::Resource::CubeGreen, {.tag = "Movable Object"});
            head.add<Collider>(BoxProperties{}, false);
            head.add<PhysicsBody>(PhysicsProperties{.mass = 5.0f});
            head.add<FixedLogic>(ForceBasedMovement);

            auto segment1 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -0.9f}, .scale = Vector3::Splat(0.8f)});
            segment1.add<Collider>(BoxProperties{}, false);
            segment1.add<PhysicsBody>(PhysicsProperties{.mass = 3.0f});

            auto segment2 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -1.6f}, .scale = Vector3::Splat(0.6f)});
            segment2.add<Collider>(BoxProperties{}, false);
            segment2.add<PhysicsBody>(PhysicsProperties{.mass = 1.0f});

            auto segment3 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -2.1f}, .scale = Vector3::Splat(0.4f)});
            segment3.add<Collider>(BoxProperties{}, false);
            segment3.add<PhysicsBody>(PhysicsProperties{.mass = 0.2f});

            float bias = 0.2f;
            float softness = 0.1f;

            physics->AddJoint(head.entity(), segment1.entity(), Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
            physics->AddJoint(segment1.entity(), segment2.entity(), Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
            physics->AddJoint(segment2.entity(), segment3.entity(), Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);
        }

        // Ground
        auto platform1 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{0.0f, -1.0f, 0.0f}, .scale = Vector3{30.0f, 1.0f, 30.0f}, .tag = "Platform1"});
        platform1.add<Collider>(BoxProperties{}, false);
        platform1.add<PhysicsBody>(PhysicsProperties{.mass = 0.0f, .isKinematic = true});

        auto platform2 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{8.0f, -1.0f, 30.0f}, .scale = Vector3{10.0f, 1.0f, 10.0f}, .tag = "Platform2"});
        platform2.add<Collider>(BoxProperties{}, false);
        platform2.add<PhysicsBody>(PhysicsProperties{.mass = 0.0f, .isKinematic = true});

        // Ramp
        {
            auto ramp = prefab::Create(registry, prefab::Resource::RampRed, {.position = Vector3{9.0f, 2.6f, 6.0f}, .scale = Vector3::Splat(3.0f), .flags = Entity::Flags::Static});
            ramp.add<ConcaveCollider>("ramp.nca");
        }

        // Hinge
        {
            auto anchor = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 3.25f, 10.0f}, .scale = Vector3{3.0f, 0.5f, 0.5f}});
            anchor.add<Collider>(BoxProperties{}, false);
            anchor.add<PhysicsBody>(PhysicsProperties{.isKinematic = true});

            auto panel = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.5f, 10.0f}, .scale = Vector3{3.0f, 3.0f, 0.1f}});
            panel.add<Collider>(BoxProperties{}, false);
            panel.add<PhysicsBody>(PhysicsProperties{});

            physics->AddJoint(anchor.entity(), panel.entity(), Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
            physics->AddJoint(anchor.entity(), panel.entity(), Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
        }

        // Balance platform
        {
            auto base = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{10.0f, -0.75f, -10.0f}, .scale = Vector3::Splat(2.0f)});
            base.add<Collider>(SphereProperties{}, false);
            base.add<PhysicsBody>(PhysicsProperties{.isKinematic = true}, Vector3::One(), Vector3::Zero());

            auto balancePlatform = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{10.0f, -0.4f, -10.0f}, .scale = Vector3{6.0f, 0.1f, 6.0f}});
            balancePlatform.add<Collider>(BoxProperties{}, false);
            balancePlatform.add<PhysicsBody>(PhysicsProperties{.mass = 5.0f});

            physics->AddJoint(base.entity(), balancePlatform.entity(), Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
        }

        // Swinging bars
        {
            auto pole = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{0.05f, 4.0f, 0.05f}});
            pole.add<Collider>(BoxProperties{}, true);
            pole.add<PhysicsBody>(PhysicsProperties{.isKinematic = true});

            auto bar1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, -0.5f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
            bar1.add<Collider>(BoxProperties{}, false);
            bar1.add<PhysicsBody>(PhysicsProperties{}, Vector3::One(), Vector3::Up());

            auto bar2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
            bar2.add<Collider>(BoxProperties{}, false);
            bar2.add<PhysicsBody>(PhysicsProperties{}, Vector3::One(), Vector3::Up());

            physics->AddJoint(pole.entity(), bar1.entity(), Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
            physics->AddJoint(pole.entity(), bar2.entity(), Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
        }

        // Bridge
        {
            auto plank1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 16.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            plank1.add<Collider>(BoxProperties{}, false);
            plank1.add<PhysicsBody>(PhysicsProperties{});

            auto plank2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 18.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            plank2.add<Collider>(BoxProperties{}, false);
            plank2.add<PhysicsBody>(PhysicsProperties{});

            auto plank3 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 20.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            plank3.add<Collider>(BoxProperties{}, false);
            plank3.add<PhysicsBody>(PhysicsProperties{});

            auto plank4 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 22.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            plank4.add<Collider>(BoxProperties{}, false);
            plank4.add<PhysicsBody>(PhysicsProperties{});

            auto plank5 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 24.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            plank5.add<Collider>(BoxProperties{}, false);
            plank5.add<PhysicsBody>(PhysicsProperties{});

            float bias = 0.2f;
            float softness = 0.5f;

            physics->AddJoint(platform1.entity(), plank1.entity(), Vector3{5.0f, 0.0f, 15.1f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics->AddJoint(platform1.entity(), plank1.entity(), Vector3{11.0f, 0.0f, 15.1f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics->AddJoint(plank1.entity(), plank2.entity(), Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics->AddJoint(plank1.entity(), plank2.entity(), Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics->AddJoint(plank2.entity(), plank3.entity(), Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics->AddJoint(plank2.entity(), plank3.entity(), Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics->AddJoint(plank3.entity(), plank4.entity(), Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics->AddJoint(plank3.entity(), plank4.entity(), Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics->AddJoint(plank4.entity(), plank5.entity(), Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics->AddJoint(plank4.entity(), plank5.entity(), Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics->AddJoint(plank5.entity(), platform2.entity(), Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -5.1f}, bias, softness);
            physics->AddJoint(plank5.entity(), platform2.entity(), Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -5.1f}, bias, softness);
        }
    }

    void JointsTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
}