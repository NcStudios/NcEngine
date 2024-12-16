#include "GraphicsTest.h"
#include "shared/Prefabs.h"
#include "shared/GameLogic.h"

#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/FrameLogic.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/RigidBody.h"

#include "ncutility/Hash.h"

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
    modules.Get<graphics::NcGraphics>()->SetSkybox(cubemap::NightSkyPath);

    // Lights
    auto lvHandle = world.Emplace<Entity>({.position = Vector3{-4.5f, 8.0f, 5.4f}, .tag = "Point Light 1"});
    world.Emplace<graphics::PointLight>(lvHandle, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.946f, 0.671f, 0.278f), 26.6f);
    auto lv2Handle = world.Emplace<Entity>({.position = Vector3{6.5f, 9.0f, 9.6f}, .tag = "Point Light 2"});
    world.Emplace<graphics::PointLight>(lv2Handle, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.723f, 0.608f), 13.4f);
    auto lv3Handle = world.Emplace<Entity>({.position = Vector3{4.5f, 6.0f, -8.4f}, .tag = "Point Light 3"});
    world.Emplace<graphics::PointLight>(lv3Handle, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 7.3f);

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
        .position = Vector3{-0.6f, 6.562f, -18.848f},
        .rotation = Quaternion::FromEulerAngles(0.239f, 0.0f, 0.021f),
        .tag = "Main Camera"
    });
    auto& camera = world.Emplace<graphics::SceneNavigationCamera>(cameraHandle);
    world.Emplace<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(&camera);
}

void GraphicsTest::Unload()
{
}
} // namespace nc::sample
