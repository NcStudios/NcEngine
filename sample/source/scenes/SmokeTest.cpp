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
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/Constraints.h"
#include "ncengine/physics/PhysicsBody.h"
#include "ncengine/physics/PhysicsMaterial.h"
#include "ncengine/physics/NcPhysics.h"
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

void LoadScene(nc::ecs::Ecs world, nc::asset::NcAsset& ncAsset)
{
    auto file = std::ifstream{g_sceneFragment, std::ios::binary};
    if (!file)
    {
        throw nc::NcError{fmt::format("Failed to open fragment for reading: '{}'", g_sceneFragment)};
    }

    nc::LoadSceneFragment(file, world, ncAsset);
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

    std::cerr << "SmokeTest::Load\n";

    (void)world;
    (void)modules;
    static auto isSecondPass = false;
    world.Emplace<FrameLogic>(
        world.Emplace<Entity>({}),
        [world, modules, quit = m_quitEngine](Entity, Registry*, float) mutable
        {
            static auto framesRun = 0ull;
            
            std::cerr << "frame" << framesRun << "\n";
            

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
        ::LoadScene(world, *modules.Get<asset::NcAsset>());
    }
    else
    {
        const auto ground = world.Emplace<Entity>({
            .position = Vector3::Up() * -2.5f,
            .scale = Vector3{3.0f, 1.0f, 3.0f}
        });

        world.Emplace<graphics::ToonRenderer>(ground);
        auto& groundCollider = world.Emplace<physics::Collider>(ground, physics::BoxProperties{});
        auto& groundTransform = world.Get<Transform>(ground);
        world.Emplace<physics::PhysicsBody>(ground, groundTransform, groundCollider);
        world.Emplace<physics::PositionClamp>(ground, groundTransform.Position(), 0.1f, 2.0f);
    }

    const auto cameraHandle = world.Emplace<Entity>({.position = Vector3{0.0f, 0.0f, -15.0f}});
    auto& camera = world.Emplace<graphics::Camera>(cameraHandle);
    auto ncGraphics = modules.Get<graphics::NcGraphics>();
    ncGraphics->SetCamera(&camera);
    ncGraphics->SetSkybox(asset::DefaultSkyboxCubeMap);
    modules.Get<audio::NcAudio>()->RegisterListener(cameraHandle);

    const auto object = world.Emplace<Entity>({});
    world.Emplace<audio::AudioSource>(
        object,
        std::vector<std::string>{
            std::string{asset::DefaultAudioClip}
        },
        audio::AudioSourceProperties{
            .flags = audio::AudioSourceFlags::Play |
                     audio::AudioSourceFlags::Loop
        }
    );

    world.Emplace<graphics::MeshRenderer>(object);
    world.Emplace<graphics::ToonRenderer>(object);
    world.Emplace<graphics::SkeletalAnimator>(object, "DefaultCube.nca", "DefaultCubeAnimation.nca");
    world.Emplace<graphics::PointLight>(object);
    world.Emplace<graphics::ParticleEmitter>(object, graphics::ParticleInfo{
        .emission = {
            .initialEmissionCount = 10,
            .periodicEmissionCount = 10,
            .periodicEmissionFrequency = 0.1f
        },
        .init = {},
        .kinematic = {}
    });

    auto& objectCollider = world.Emplace<physics::Collider>(object, physics::BoxProperties{});
    auto& objectTransform = world.Get<Transform>(object);
    world.Emplace<physics::PhysicsBody>(object, objectTransform, objectCollider);
    world.Emplace<physics::PhysicsMaterial>(object);
    world.Emplace<physics::VelocityRestriction>(object, Vector3::One(), Vector3::Up());

    world.Emplace<Entity>({.parent = object});
}
} // namespace nc::sample
