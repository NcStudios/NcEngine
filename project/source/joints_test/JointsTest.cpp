#include "JointsTest.h"
#include "Ecs.h"
#include "MainCamera.h"
#include "shared/Prefabs.h"
#include "ForceBasedController.h"
#include "shared/SceneNavigationCamera.h"
#include "Physics.h"
#include "Audio.h"

namespace nc::sample
{
    void JointsTest::Load(registry_type* registry)
    {
        m_sceneHelper.Setup(registry, false, false, nullptr);

        // Camera
        auto cameraHandle = registry->Add<Entity>({.position = Vector3{0.0f, 6.1f, -6.5f}, .rotation = Quaternion::FromEulerAngles(0.7f, 0.0f, 0.0f), .tag = "Main Camera"});
        auto camera = registry->Add<SceneNavigationCamera>(cameraHandle, 0.05f, 0.005f, 1.4f);
        camera::SetMainCamera(camera);
        audio::RegisterListener(cameraHandle);

        // Lights
        auto lvHandle = registry->Add<Entity>({.position = Vector3{0.0f, 3.4f, 1.3f}, .tag = "Point Light 1"});
        registry->Add<PointLight>(lvHandle, PointLightInfo{.pos = Vector3::Zero(),
                                                           .ambient = Vector3{0.443f, 0.412f, 0.412f},
                                                           .diffuseColor = Vector3{0.4751, 0.525f, 1.0f},
                                                           .diffuseIntensity = 3.0,
                                                           .attConst = 0.0f,
                                                           .attLin = 0.05f,
                                                           .attQuad = 0.0f});

        // Movable object
        {
            auto head = prefab::Create(registry, prefab::Resource::CubeGreen, {.tag = "Movable Object"});
            registry->Add<Collider>(head, BoxProperties{}, false);
            registry->Add<PhysicsBody>(head, PhysicsProperties{.mass = 5.0f});
            registry->Add<ForceBasedController>(head, registry);
            auto* audio = registry->Add<AudioSource>(head, "project/assets/sounds/drums.wav", AudioSourceProperties{.innerRadius = 5.0f, .outerRadius = 25.0f, .spatialize = true, .loop = true});
            audio->Play();

            auto segment1 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -0.9f}, .scale = Vector3::Splat(0.8f)});
            registry->Add<Collider>(segment1, BoxProperties{}, false);
            registry->Add<PhysicsBody>(segment1, PhysicsProperties{.mass = 3.0f});

            auto segment2 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -1.6f}, .scale = Vector3::Splat(0.6f)});
            registry->Add<Collider>(segment2, BoxProperties{}, false);
            registry->Add<PhysicsBody>(segment2, PhysicsProperties{.mass = 1.0f});

            auto segment3 = prefab::Create(registry, prefab::Resource::CubeGreen, {.position = Vector3{0.0f, 0.0f, -2.1f}, .scale = Vector3::Splat(0.4f)});
            registry->Add<Collider>(segment3, BoxProperties{}, false);
            registry->Add<PhysicsBody>(segment3, PhysicsProperties{.mass = 0.2f});

            float bias = 0.2f;
            float softness = 0.1f;

            physics::AddJoint(head, segment1, Vector3{0.0f, 0.0f, -0.6f}, Vector3{0.0f, 0.0f, 0.5f}, bias, softness);
            physics::AddJoint(segment1, segment2, Vector3{0.0f, 0.0f, -0.5f}, Vector3{0.0f, 0.0f, 0.4f}, bias, softness);
            physics::AddJoint(segment2, segment3, Vector3{0.0f, 0.0f, -0.4f}, Vector3{0.0f, 0.0f, 0.3f}, bias, softness);
        }

        // Ground
        auto platform1 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{0.0f, -1.0f, 0.0f}, .scale = Vector3{30.0f, 1.0f, 30.0f}, .tag = "Platform1"});
        registry->Add<Collider>(platform1, BoxProperties{}, false);
        registry->Add<PhysicsBody>(platform1, PhysicsProperties{.mass = 0.0f, .isKinematic = true});

        auto platform2 = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{8.0f, -1.0f, 30.0f}, .scale = Vector3{10.0f, 1.0f, 10.0f}, .tag = "Platform2"});
        registry->Add<Collider>(platform2, BoxProperties{}, false);
        registry->Add<PhysicsBody>(platform2, PhysicsProperties{.mass = 0.0f, .isKinematic = true});

        // Ramp
        {
            auto ramp = prefab::Create(registry, prefab::Resource::RampRed, {.position = Vector3{9.0f, 2.6f, 6.0f}, .scale = Vector3::Splat(3.0f), .flags = Entity::Flags::Static});
            registry->Add<ConcaveCollider>(ramp, "project/assets/mesh_colliders/ramp.nca");
        }

        // Hinge
        {
            auto anchor = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 3.25f, 10.0f}, .scale = Vector3{3.0f, 0.5f, 0.5f}});
            registry->Add<Collider>(anchor, BoxProperties{}, false);
            registry->Add<PhysicsBody>(anchor, PhysicsProperties{.isKinematic = true});

            auto panel = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.5f, 10.0f}, .scale = Vector3{3.0f, 3.0f, 0.1f}});
            registry->Add<Collider>(panel, BoxProperties{}, false);
            registry->Add<PhysicsBody>(panel, PhysicsProperties{});

            physics::AddJoint(anchor, panel, Vector3{-2.0f, -0.255f, 0.0f}, Vector3{-2.0f, 1.55f, 0.0f});
            physics::AddJoint(anchor, panel, Vector3{2.0f, -0.255f, 0.0f}, Vector3{2.0f, 1.55f, 0.0f});
        }

        // Balance platform
        {
            auto base = prefab::Create(registry, prefab::Resource::SphereRed, {.position = Vector3{10.0f, -0.75f, -10.0f}, .scale = Vector3::Splat(2.0f)});
            registry->Add<Collider>(base, SphereProperties{}, false);
            registry->Add<PhysicsBody>(base, PhysicsProperties{.isKinematic = true}, Vector3::One(), Vector3::Zero());

            auto balancePlatform = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{10.0f, -0.4f, -10.0f}, .scale = Vector3{6.0f, 0.1f, 6.0f}});
            registry->Add<Collider>(balancePlatform, BoxProperties{}, false);
            registry->Add<PhysicsBody>(balancePlatform, PhysicsProperties{.mass = 5.0f});

            physics::AddJoint(base, balancePlatform, Vector3{0.0f, 1.1f, 0.0f}, Vector3{0.0f, -0.15f, 0.0f}, 0.2f, 0.1f);
        }

        // Swinging bars
        {
            auto pole = prefab::Create(registry, prefab::Resource::CubeBlue, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{0.05f, 4.0f, 0.05f}});
            registry->Add<Collider>(pole, BoxProperties{}, true);
            registry->Add<PhysicsBody>(pole, PhysicsProperties{.isKinematic = true});

            auto bar1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, -0.5f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
            registry->Add<Collider>(bar1, BoxProperties{}, false);
            registry->Add<PhysicsBody>(bar1, PhysicsProperties{}, Vector3::One(), Vector3::Up());

            auto bar2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{-10.0f, 1.0f, -10.0f}, .scale = Vector3{3.0f, 1.0f, 0.1f}});
            registry->Add<Collider>(bar2, BoxProperties{}, false);
            registry->Add<PhysicsBody>(bar2, PhysicsProperties{}, Vector3::One(), Vector3::Up());

            physics::AddJoint(pole, bar1, Vector3{0.0f, -0.5f, 0.0f}, Vector3{});
            physics::AddJoint(pole, bar2, Vector3{0.0f, 1.0f, 0.0f}, Vector3{});
        }

        // Bridge
        {
            auto plank1 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 16.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            registry->Add<Collider>(plank1, BoxProperties{}, false);
            registry->Add<PhysicsBody>(plank1, PhysicsProperties{});

            auto plank2 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 18.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            registry->Add<Collider>(plank2, BoxProperties{}, false);
            registry->Add<PhysicsBody>(plank2, PhysicsProperties{});

            auto plank3 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 20.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            registry->Add<Collider>(plank3, BoxProperties{}, false);
            registry->Add<PhysicsBody>(plank3, PhysicsProperties{});

            auto plank4 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 22.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            registry->Add<Collider>(plank4, BoxProperties{}, false);
            registry->Add<PhysicsBody>(plank4, PhysicsProperties{});

            auto plank5 = prefab::Create(registry, prefab::Resource::CubeRed, {.position = Vector3{8.0f, -1.0f, 24.0f}, .scale = Vector3{8.0f, 0.8f, 1.8f}, .tag = "Plank"});
            registry->Add<Collider>(plank5, BoxProperties{}, false);
            registry->Add<PhysicsBody>(plank5, PhysicsProperties{});

            float bias = 0.2f;
            float softness = 0.5f;

            physics::AddJoint(platform1, plank1, Vector3{5.0f, 0.0f, 15.1f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics::AddJoint(platform1, plank1, Vector3{11.0f, 0.0f, 15.1f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics::AddJoint(plank1, plank2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics::AddJoint(plank1, plank2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics::AddJoint(plank2, plank3, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics::AddJoint(plank2, plank3, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics::AddJoint(plank3, plank4, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics::AddJoint(plank3, plank4, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics::AddJoint(plank4, plank5, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -1.0f}, bias, softness);
            physics::AddJoint(plank4, plank5, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -1.0f}, bias, softness);

            physics::AddJoint(plank5, platform2, Vector3{-3.0f, 0.0f, 1.0f}, Vector3{-3.0f, 0.0f, -5.1f}, bias, softness);
            physics::AddJoint(plank5, platform2, Vector3{3.0f, 0.0f, 1.0f}, Vector3{3.0f, 0.0f, -5.1f}, bias, softness);
        }
    }

    void JointsTest::Unload()
    {
        m_sceneHelper.TearDown();
    }
}