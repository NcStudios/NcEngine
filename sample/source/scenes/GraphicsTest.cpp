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

#include <string>

namespace nc::sample
{
GraphicsTest::GraphicsTest(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void GraphicsTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(nullptr);
    modules.Get<NcGraphics>()->SetSkybox(cubemap::NightSkyPath);

    // Lights
    auto lvHandle = world.Emplace<Entity>({.position = Vector3{1.5f, 9.0f, 5.4f}, .tag = "Point Light 1"});
    world.Emplace<PointLight>(lvHandle, Vector3(0.489f, 0.784f, 0.804f), Vector3(1.0f, 0.745f, 0.910f), Vector3(0.265f, 0.514f, 0.529f), 23.7f);

    const auto guy = world.Emplace<Entity>({
        .position = Vector3{9.0f, 0.3f, 4.0f},
        .rotation = Quaternion::FromEulerAngles(1.579f, 1.322f, 0.091f),
        .scale = Vector3{3.0f, 3.0f, 3.0f},
        .tag = "guy"
    });
    
    world.Emplace<StaticMesh>(guy, mesh::Guy, material::Guy);

    const auto guy2 = world.Emplace<Entity>({
        .position = Vector3{6.0f, 0.3f, 4.0f},
        .rotation = Quaternion::FromEulerAngles(1.579f, 1.322f, 0.091f),
        .scale = Vector3{3.0f, 3.0f, 3.0f},
        .tag = "guy2"
    });
    world.Emplace<StaticMesh>(guy2, mesh::Guy2, material::Guy2);

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
            mesh::Ogre,
            material::Ogre,
            animation::OgreIdle
        ).GetAnimationController();

        const auto stopState = animator.AddState(StopAnimation{
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .enterFrom = RootAnimationState
        });

        animator.AddState(LoopAnimation{
            .animId = animation::OgreIdle,
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
                auto& ogreAnim = ecs.Get<SkinnedMesh>(other).GetAnimationController();
                ogreAnim.PlayOnceImmediate(animation::OgreAttack);
                auto& tag = ecs.Get<Tag>(other);
                GameLog::Log(fmt::format("Collision Enter: {}", tag.value));
            };

        auto& animator = world.Emplace<SkinnedMesh>(
            skeleton,
            mesh::Skeleton,
            material::Skeleton,
            animation::SkeletonIdle
        ).GetAnimationController();

        animator.AddState(LoopAnimation{
            .animId = animation::SkeletonWalkForward,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::W);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::W);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::SkeletonWalkLeft,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::A);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::A);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::SkeletonWalkBackward,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::S);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::S);}
        });

        animator.AddState(LoopAnimation{
            .animId = animation::SkeletonWalkRight,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::D);},
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::D);}
        });

        animator.AddState(PlayOnceAnimation{
            .animId = animation::SkeletonJump,
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

    world.Emplace<StaticMesh>(cave_floor, mesh::Cave, material::Cave);

    // Camera
    auto cameraHandle = world.Emplace<Entity>({
        .position = Vector3{0.854f, 2.612f, 2.234f},
        .rotation = Quaternion::FromEulerAngles(0.318f, 1.269f, 0.07f),
        .tag = "Main Camera"
    });

    auto& camera = world.Emplace<SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<SceneNavigationCamera>{});
    auto ncGraphics = modules.Get<NcGraphics>();
    ncGraphics->SetCamera(&camera);
    ncGraphics->SetPostProcessEffectEnabled(nc::OutlinedToonEffectId, true);
    ncGraphics->SetPostProcessEffectProperties(nc::OutlinedToonEffectId, PostProcessPassFlag::Outline, OutlinePassProperties{.width = 0.3f, .depthThreshold = 0.8f, .normalThreshold = 0.120f});
}

void GraphicsTest::Unload()
{
}
} // namespace nc::sample
