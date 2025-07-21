#include "GraphicsTest.h"
#include "shared/Prefabs.h"
#include "shared/GameLogic.h"

#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/FrameLogic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/RigidBody.h"
#include "shared/GeneratedAssets.h"

#include <string>

namespace
{
void ChangeShapeKey(nc::Entity self, nc::ecs::Ecs world, float )
{
    if (nc::input::KeyDown(nc::input::KeyCode::Two))
    {
        auto& mesh = world.Get<nc::StaticMesh>(self);
        mesh.SetShapeKeyAnimation(nc::sample::shapekey_animation::chute);
    }
    else if (nc::input::KeyDown(nc::input::KeyCode::Three))
    {
        auto& mesh = world.Get<nc::StaticMesh>(self);
        mesh.SetShapeKeyAnimation(nc::sample::shapekey_animation::fall);
    }
}
} // anonymous namespace

namespace nc::sample
{
const nc::Vector3 GraphicsTest::Extents = nc::Vector3{20.0f, 20.0f, 40.0f};

GraphicsTest::GraphicsTest(SampleUI* ui, Vector3 extents)
    : Scene{extents},
      m_sampleUI{ui}
{
}

void GraphicsTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    ReloadPrefabs();
    m_sampleUI->SetWidgetCallback(nullptr);
    modules.Get<NcGraphics>()->SetSkybox(cube_map::path::night_sky);

    // Lights
    auto lvHandle = world.Emplace<Entity>({.position = Vector3{3.1f, 6.2f, 4.5f}, .tag = "Point Light 1"});
    world.Emplace<PointLight>(lvHandle, Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), 1.0f, 45.0f);

    const auto guy2 = world.Emplace<Entity>({
        .position = Vector3{6.0f, 1.8f, 4.0f},
        .rotation = Quaternion::FromEulerAngles(1.579f, 1.322f, 0.091f),
        .scale = Vector3{2.75f, 2.75f, 2.75f},
        .tag = "guy2"
    });
    world.Emplace<StaticMesh>(guy2, mesh::guy2, material::guy2);

    const auto flag = world.Emplace<Entity>({
        .position = Vector3{5.0f, -3.0f, 11.0f},
        .rotation = Quaternion::FromEulerAngles(0.056f, 0.230f, -0.047f),
        .scale = Vector3{2.0f, 2.0f, 2.0f},
        .tag = "Flag"
    });
    world.Emplace<StaticMesh>(flag, mesh::flag, material::blue, shapekey_animation::flag);

    const auto dress = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "Dress"
    });
    world.Emplace<StaticMesh>(dress, mesh::dress, material::red, shapekey_animation::twirl);

    const auto girl = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "girl"
    });
    world.Emplace<SkinnedMesh>(girl, mesh::girlglb, material::blue, animation::dance);
    
    const auto steeple = world.Emplace<Entity>({
        .position = Vector3{8.0f, 0.3f, 1.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{2.9f, 1.8f, 3.0f},
        .tag = "steeple"
    });
    world.Emplace<StaticMesh>(steeple, mesh::steeple, material::blue);

    const auto sheet = world.Emplace<Entity>({
        .position = Vector3{8.0f, 0.0f, 1.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "sheet"
    });
    world.Emplace<StaticMesh>(sheet, mesh::sheet, material::yellow, shapekey_animation::chute);
    world.Emplace<FrameLogic>(sheet, ChangeShapeKey);

    // Ogre
    {
        const auto ogre = world.Emplace<Entity>({
            .position = Vector3{-5.0f, 0.0f, 12.0f},
            .rotation = Quaternion::FromEulerAngles(0.0f, 1.0f, 0.0f),
            .scale = Vector3{3.0f, 3.0f, 3.0f},
            .tag = "ogre"
        });

        world.Emplace<RigidBody>(
            ogre,
            Shape::MakeSphere(),
            RigidBodyInfo{
                .type = BodyType::Kinematic
            }
        );

        auto& animator = world.Emplace<SkinnedMesh>(
            ogre,
            mesh::ogre,
            material::ogre,
            animation::ogre_idle
        ).GetSkeletalAnimationController();

        const auto stopState = animator.AddState(StopAnimation{
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .enterFrom = RootAnimationState
        });

        animator.AddState(LoopAnimation{
            .animId = animation::ogre_idle,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .enterFrom = stopState,
            .exitWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .exitTo = stopState
        });
    }

    // Skeleton
    {
        const auto skeleton = world.Emplace<Entity>({
            .position = Vector3{5.3f, 0.0f, -6.4f},
            .rotation = Quaternion::FromEulerAngles(0.0f, 0.5f, 0.0f),
            .scale = Vector3{2.0f, 2.0f, 2.0f},
            .tag = "skeleton"
        });

        world.Emplace<FrameLogic>(skeleton, WasdBasedSimulatedBodyMovement);
        world.Emplace<RigidBody>(
            skeleton,
            Shape::MakeSphere(),
            RigidBodyInfo{
                .type = BodyType::Kinematic,
                .flags = RigidBodyFlags::Trigger
            }
        );

        world.Emplace<CollisionListener>(skeleton)
            .onTriggerEnter = [](Entity, Entity other, ecs::Ecs ecs){
                auto& ogreAnim = ecs.Get<SkinnedMesh>(other).GetSkeletalAnimationController();
                ogreAnim.PlayOnceImmediate(animation::ogre_attack);
                auto& tag = ecs.Get<Tag>(other);
                GameLog::Log(fmt::format("Collision Enter: {}", tag.value));
            };

        auto& animator = world.Emplace<SkinnedMesh>(
            skeleton,
            mesh::skeleton,
            material::skeleton,
            animation::skeleton_idle
        ).GetSkeletalAnimationController();

        animator.AddState(LoopAnimation{
            .animId = animation::skeleton_walk_forward,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::W);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::W);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::skeleton_walk_left,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::A);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::A);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::skeleton_walk_back,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::S);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::S);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::skeleton_walk_right,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::D);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::D);}
        });

        animator.AddState(PlayOnceAnimation{
            .animId = animation::skeleton_jump,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::Space);}
        });
    }

    // Cave
    const auto cave_floor = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.5708f, 0.0f),
        .scale = Vector3{1.5f, 1.5f, 1.5f},
        .tag = "cave_floor"
    });

    world.Emplace<StaticMesh>(cave_floor, mesh::cave, material::cave);

    // Camera
    auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.137f, 5.291f, 1.245f},
        .rotation = Quaternion::FromEulerAngles(0.549f, 1.116f, 0.146f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<SceneNavigationCamera>{});
    auto ncGraphics = modules.Get<NcGraphics>();
    ncGraphics->SetCamera(&camera);
    ncGraphics->SetPostProcessEffectEnabled(nc::OutlinedToonEffectId, true);
    ncGraphics->SetPostProcessEffectProperties(nc::OutlinedToonEffectId, PostProcessPassFlag::Outline, OutlinePassProperties
    {
        .width = 2.0f,
        .depthThreshold = 3.69f,
        .viewDirDepthThreshold = 0.04f,
        .normalThreshold = 0.920f
    });

    ncGraphics->SetPostProcessEffectProperties(nc::OutlinedToonEffectId, PostProcessPassFlag::Noise, NoisePassProperties
    {
        .maskGradientStart = Vector3{1.0f, 1.0f, 1.0f},
        .maskGradientAmount = 1.0f,
        .maskGradientEnd = Vector3{0.0f, 0.0f, 0.0f},
        .noiseTex = texture::effect_noise,
        .noiseTexAmount = 0.24f,
        .noiseTexTiling = 1.0f,
    });
}

void GraphicsTest::Unload()
{
}
} // namespace nc::sample
