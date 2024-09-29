#include "SmokeTest.h"

#include "ncengine/asset/Assets.h"
#include "ncengine/asset/DefaultAssets.h"
#include "ncengine/audio/AudioSource.h"
#include "ncengine/audio/NcAudio.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/NcPhysics.h"
#include "ncengine/physics/RigidBody.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/serialize/SceneSerialization.h"

#include <fstream>

namespace
{
constexpr auto g_sceneFragment = "smokeTestScene";

void SaveScene(nc::ecs::Ecs world, const nc::asset::AssetMap& assets)
{
    auto file = std::ofstream{g_sceneFragment, std::ios::binary | std::ios::trunc};
    if (!file)
    {
        throw nc::NcError{fmt::format("Failed to open fragment for writing: '{}'", g_sceneFragment)};
    }

    nc::SaveSceneFragment(file, world, assets);
}

void LoadScene(nc::ecs::Ecs world, nc::ModuleProvider modules)
{
    auto file = std::ifstream{g_sceneFragment, std::ios::binary};
    if (!file)
    {
        throw nc::NcError{fmt::format("Failed to open fragment for reading: '{}'", g_sceneFragment)};
    }

    nc::LoadSceneFragment(file, world, modules);
}
} // anonymous namespace

namespace nc::sample
{
SmokeTest::SmokeTest(std::function<void()> quitEngineCallback)
    : m_quitEngine{std::move(quitEngineCallback)}
{
}

void SmokeTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    // Set up for a coarse integration test. We add a few components and set module state so that each system runs
    // its primary logic. After a few frames, we save a scene fragment and reload the scene, this time also reading
    // the fragment. After a few more frames, we quit. Ideally, this should be run with validation layers enabled.

    world.GetPool<physics::RigidBody>().Reserve(30ull);
    static auto isSecondPass = false;
    world.Emplace<FrameLogic>(
        world.Emplace<Entity>({}),
        [world, modules, quit = m_quitEngine](Entity, Registry*, float) mutable
        {
            static auto framesRun = 0ull;
            constexpr auto framesUntilSceneChange = 60ull;
            constexpr auto framesUntilQuit = 120ull;
            ++framesRun;
            if (framesRun == framesUntilSceneChange)
            {
                isSecondPass = true;
                auto ncAsset = modules.Get<asset::NcAsset>();
                ::SaveScene(world, ncAsset->GetLoadedAssets());
                auto ncScene = modules.Get<NcScene>();
                ncScene->Queue(std::make_unique<SmokeTest>(quit));
                ncScene->ScheduleTransition();
            }
            else if (framesRun == framesUntilQuit)
            {
                quit();
            }
        }
    );

    if (isSecondPass)
    {
        asset::UnloadAllAudioClipAssets();
        asset::UnloadAllConcaveColliderAssets();
        asset::UnloadAllConvexHullAssets();
        asset::UnloadAllCubeMapAssets();
        asset::UnloadAllMeshAssets();
        asset::UnloadAllTextureAssets();
        asset::UnloadAllSkeletalAnimationAssets();
        ::LoadScene(world, modules);
    }
    else
    {
        const auto ground = world.Emplace<Entity>({
            .position = Vector3::Up() * -3.5f,
            .scale = Vector3{10.0f, 1.0f, 10.0f}
        });

        world.Emplace<graphics::ToonRenderer>(ground);
        auto& groundBody = world.Emplace<physics::RigidBody>(
            ground,
            physics::Shape::MakeBox(),
            physics::RigidBodyInfo{
                .freedom = physics::DegreeOfFreedom::Rotation
            }
        );

        groundBody.AddConstraint(
            physics::SwingTwistConstraintInfo{
                .ownerTwistAxis = Vector3::Up(),
                .targetTwistAxis = Vector3::Up(),
                .swingLimit = 0.1f
            }
        );

        const auto trigger = world.Emplace<Entity>({
            .position = Vector3::Down() * 3.0f,
            .scale = Vector3{9.0f, 0.5f, 9.0f}
        });

        world.Emplace<graphics::WireframeRenderer>(
            trigger,
            graphics::WireframeSource::Collider,
            trigger,
            Vector4{0.9f, 0.1f, 0.7f, 1.0f}
        );

        auto& triggerBody = world.Emplace<physics::RigidBody>(
            trigger,
            physics::Shape::MakeBox(),
            physics::RigidBodyInfo{
                .flags = physics::RigidBodyFlags::Trigger
            }
        );

        triggerBody.AddConstraint(
            physics::FixedConstraintInfo{
                .ownerPosition = Vector3::Down() * 0.25f,
                .targetPosition = Vector3::Up() * 0.5f
            },
            groundBody
        );
    }

    const auto cameraHandle = world.Emplace<Entity>({.position = Vector3{0.0f, 0.0f, -15.0f}});
    auto& camera = world.Emplace<graphics::Camera>(cameraHandle);
    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    ncGraphics->SetCamera(&camera);
    ncGraphics->SetSkybox(asset::DefaultSkyboxCubeMap);
    modules.Get<audio::NcAudio>()->RegisterListener(cameraHandle);

    const auto animatedCube = world.Emplace<Entity>({
        .position = Vector3::Up() * 4.0f
    });

    world.Emplace<graphics::MeshRenderer>(animatedCube);
    world.Emplace<graphics::ToonRenderer>(animatedCube);
    world.Emplace<graphics::SkeletalAnimator>(animatedCube, "DefaultCube.nca", "DefaultCubeAnimation.nca");
    world.Emplace<audio::AudioSource>(
        animatedCube,
        std::vector<std::string>{
            std::string{asset::DefaultAudioClip}
        },
        audio::AudioSourceProperties{
            .flags = audio::AudioSourceFlags::Play |
                     audio::AudioSourceFlags::Loop
        }
    );

    const auto pointLight = world.Emplace<Entity>({
        .position = Vector3::Down(),
        .parent = animatedCube
    });

    world.Emplace<graphics::PointLight>(pointLight);

    const auto box1 = world.Emplace<Entity>({.position = Vector3{-1.0f, 0.0f, 0.0f}});
    const auto box2 = world.Emplace<Entity>({.position = Vector3{1.0f, 0.0f, 0.0f}});
    const auto sphere1 = world.Emplace<Entity>({.position = Vector3{-4.0f, 1.0f, 0.0f}});
    const auto sphere2 = world.Emplace<Entity>({.position = Vector3{-3.0f, 0.0f, 0.0f}});
    const auto capsule1 = world.Emplace<Entity>({.position = Vector3{3.5f, 0.0f, 0.0f}});
    const auto capsule2 = world.Emplace<Entity>({.position = Vector3{3.5f, 3.0f, 0.0f}});

    world.Emplace<graphics::MeshRenderer>(box1, asset::CubeMesh);
    world.Emplace<graphics::MeshRenderer>(box2, asset::CubeMesh);
    world.Emplace<graphics::ToonRenderer>(sphere1, asset::SphereMesh);
    world.Emplace<graphics::ToonRenderer>(sphere2, asset::SphereMesh);
    world.Emplace<graphics::ToonRenderer>(capsule1, asset::CapsuleMesh);
    world.Emplace<graphics::ToonRenderer>(capsule2, asset::CapsuleMesh);

    auto& box1Body = world.Emplace<physics::RigidBody>(box1);
    auto& box2Body = world.Emplace<physics::RigidBody>(box2);
    auto& sphere1Body = world.Emplace<physics::RigidBody>(sphere1);
    auto& sphere2Body = world.Emplace<physics::RigidBody>(sphere2);
    auto& capsule1Body = world.Emplace<physics::RigidBody>(capsule1);
    auto& capsule2Body = world.Emplace<physics::RigidBody>(capsule2);

    world.Emplace<physics::CollisionListener>(
        box1,
        [](Entity, Entity, const physics::HitInfo&, ecs::Ecs) {},
        [](Entity, Entity, ecs::Ecs) {},
        [](Entity, Entity, ecs::Ecs) {},
        [](Entity, Entity, ecs::Ecs) {}
    );

    box1Body.AddImpulse(Vector3::Up() * 5000.0f);
    sphere1Body.AddImpulse(Vector3::Up() * 5000.0f);
    capsule1Body.AddImpulse(Vector3::Up() * 5000.0f);

    box1Body.AddConstraint(
        physics::PointConstraintInfo{
            .ownerPosition = Vector3::Right() * 0.6f,
            .targetPosition = Vector3::Left() * 0.6f
        },
        box2Body
    );

    box2Body.AddConstraint(
        physics::SliderConstraintInfo{
            .minLimit = -3.0f,
            .maxLimit = 3.0f
        },
        sphere1Body
    );

    sphere1Body.AddConstraint(
        physics::DistanceConstraintInfo{},
        sphere2Body
    );

    capsule1Body.AddConstraint(
        physics::HingeConstraintInfo{
            .ownerPosition = Vector3::Up(),
            .targetPosition = Vector3::Down()
        },
        capsule2Body
    );

    world.Emplace<Entity>({.parent = box1});
}
} // namespace nc::sample
