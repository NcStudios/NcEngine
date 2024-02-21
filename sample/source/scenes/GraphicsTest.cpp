#include "GraphicsTest.h"
#include "shared/Prefabs.h"
#include "shared/GameLogic.h"

#include "imgui/imgui.h"
#include "ncengine/NcEngine.h"
#include "ncengine/asset/Assets.h"
#include "ncengine/ecs/InvokeFreeComponent.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/SceneNavigationCamera.h"
#include "ncengine/input/Input.h"
#include "ncengine/physics/PhysicsBody.h"

#include <string>
#include <iostream>


#include "ncengine/physics/Collider.h"
#include "ncengine/ecs/Logic.h"

namespace nc::sample
{
GraphicsTest::GraphicsTest(SampleUI* ui)
    : m_sampleUI{ui}
{
}

void GraphicsTest::Load(Registry* registry, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(nullptr);

    std::vector<std::string> textures
    {
        "ogre\\BaseColor.nca",
        "ogre\\Roughness.nca",
        "ogre\\Metallic.nca",
        "cave\\BaseColor.nca",
        "cave\\Roughness.nca",
        "cave\\Metallic.nca",
        "cave_ceiling\\BaseColor.nca",
        "cave_ceiling\\Roughness.nca",
        "cave_ceiling\\Metallic.nca",
        "skeleton\\BaseColor.nca",
        "skeleton\\Roughness.nca",
        "skeleton\\Metallic.nca",
    };

    std::vector<std::string> normals
    {
        "ogre\\Normal.nca",
        "cave\\Normal.nca",
        "cave_ceiling\\Normal.nca",
        "skeleton\\Normal.nca"
    };

    std::vector<std::string> cubemaps
    {
        "night_sky.nca"
    };

    std::vector<std::string> meshes
    {
        "ogre.nca",
        "skeleton.nca",
        "cave.nca",
        "cave_ceiling.nca"
    };

    std::vector<std::string> animations
    {
        "ogre//idle.nca",
        "ogre//attack.nca",
        "skeleton//idle.nca",
        "skeleton//walk_right.nca",
        "skeleton//walk_back.nca",
        "skeleton//walk_forward.nca",
        "skeleton//walk_left.nca",
        "skeleton//jump.nca",
    };

    LoadSkeletalAnimationAssets(animations);
    LoadTextureAssets(textures, false, AssetFlags::TextureTypeImage);
    LoadTextureAssets(normals, false, AssetFlags::TextureTypeNormalMap);
    LoadMeshAssets(meshes);
    LoadCubeMapAssets(cubemaps);

    auto ogreMaterial = graphics::PbrMaterial{
        .baseColor  = "ogre\\BaseColor.nca",
        .normal     = "ogre\\Normal.nca",
        .roughness  = "ogre\\Roughness.nca",
        .metallic   = "ogre\\Metallic.nca"
    };

    auto skeletonMaterial = graphics::PbrMaterial{
        .baseColor  = "skeleton\\BaseColor.nca",
        .normal     = "skeleton\\Normal.nca",
        .roughness  = "skeleton\\Roughness.nca",
        .metallic   = "skeleton\\Metallic.nca"
    };

    auto caveMaterial = graphics::PbrMaterial{
        .baseColor  = "cave\\BaseColor.nca",
        .normal     = "cave\\Normal.nca",
        .roughness  = "cave\\Roughness.nca",
        .metallic   = "cave\\Metallic.nca"
    };

    auto caveCeilingMaterial = graphics::PbrMaterial{
        .baseColor  = "cave_ceiling\\BaseColor.nca",
        .normal     = "cave_ceiling\\Normal.nca",
        .roughness  = "cave_ceiling\\Roughness.nca",
        .metallic   = "cave_ceiling\\Metallic.nca"
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox("night_sky.nca");

    //Lights
    auto lvHandle = registry->Add<Entity>({.position = Vector3{-4.5f, 7.0f, -12.6f}, .tag = "Point Light 1"});
    registry->Add<graphics::PointLight>(lvHandle, Vector3(.238f, .441f, .334f), Vector3(.131f, .260f, .0495f), 88.6f);
    auto lv2Handle = registry->Add<Entity>({.position = Vector3{-4.5f, 5.0f, 7.6f}, .tag = "Point Light 2"});
    registry->Add<graphics::PointLight>(lv2Handle, Vector3(.279f, .036f, .036f), Vector3(.219f, .206f, .417f), 180.4f);

    // Ogre
    auto ogre = registry->Add<Entity>({
        .position = Vector3{-5.0f, 0.0f, 12.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.0f, 0.0f),
        .scale = Vector3{3.0f, 3.0f, 3.0f},
        .tag = "ogre"
    });
    registry->Add<graphics::MeshRenderer>(ogre, "ogre.nca", ogreMaterial);
    registry->Add<physics::Collider>(ogre, physics::SphereProperties{}, false);
    registry->Add<physics::PhysicsBody>(ogre, physics::PhysicsProperties{.mass = 0.0f, .isKinematic = true});

    // Ogre Animation
    {
        using namespace graphics;
        auto ogreAnimator = registry->Add<SkeletalAnimator>(ogre, "ogre.nca", "ogre//idle.nca");
        auto stopState = ogreAnimator->AddState(anim::Stop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);}
        });

        ogreAnimator->AddState(anim::Loop
        {
            .enterFrom = stopState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .animUid = "ogre//idle.nca",
            .exitWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .exitTo = stopState
        });
    }

    // Skeleton
    auto skeleton = registry->Add<Entity>({
        .position = Vector3{5.3f, 0.0f, -6.4f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.5f, 0.0f),
        .scale = Vector3{2.0f, 2.0f, 2.0f},
        .tag = "skeleton"
    });

    registry->Add<graphics::MeshRenderer>(skeleton, "skeleton.nca", skeletonMaterial);
    registry->Add<FrameLogic>(skeleton, WasdBasedMovement);
    registry->Add<physics::Collider>(skeleton, physics::SphereProperties{}, true);
    registry->Add<CollisionLogic>(skeleton, nullptr, nullptr,
    [](Entity, Entity other, Registry* reg)
    {
        auto ogreAnim = reg->Get<graphics::SkeletalAnimator>(other);
        ogreAnim->PlayOnceImmediate("ogre//attack.nca", graphics::anim::RootState);
        if(auto* tag = reg->Get<Tag>(other); tag)
        {
            GameLog::Log(std::string{"Collision Enter: "} + tag->value.c_str());
        }
    }, nullptr);

    // Skeleton Animation
    {
        using namespace graphics;
        auto skelAnim = registry->Add<SkeletalAnimator>(skeleton, "skeleton.nca", "skeleton//idle.nca");
        skelAnim->AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::W);},
            .animUid = "skeleton//walk_forward.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::W);}
        });

        skelAnim->AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::A);},
            .animUid = "skeleton//walk_left.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::A);}
        });

        skelAnim->AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::S);},
            .animUid = "skeleton//walk_back.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::S);}
        });

        skelAnim->AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::D);},
            .animUid = "skeleton//walk_right.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::D);}
        });

        skelAnim->AddState(anim::PlayOnce
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::Space);},
            .animUid = "skeleton//jump.nca"
        });
    }

    // Cave
    auto cave_floor = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.5708f, 0.0f),
        .scale = Vector3{1.5f, 1.5f, 1.5f},
        .tag = "cave_floor"
    });
    registry->Add<graphics::MeshRenderer>(cave_floor, "cave.nca", caveMaterial);

    auto cave_ceiling = registry->Add<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.5708f, 0.0f),
        .scale = Vector3{1.5f, 1.5f, 1.5f},
        .tag = "cave_ceiling"
    });
    registry->Add<graphics::MeshRenderer>(cave_ceiling, "cave_ceiling.nca", caveCeilingMaterial);

    // Camera
    auto cameraHandle = registry->Add<Entity>({
        .position = Vector3{-0.6f, 6.562f, -18.848f},
        .rotation = Quaternion::FromEulerAngles(0.239f, 0.0f, 0.021f),
        .tag = "Main Camera"
    });
    auto camera = registry->Add<graphics::SceneNavigationCamera>(cameraHandle);
    registry->Add<FrameLogic>(cameraHandle, InvokeFreeComponent<graphics::SceneNavigationCamera>{});
    modules.Get<graphics::NcGraphics>()->SetCamera(camera);
}

void GraphicsTest::Unload()
{
}
} // namespace nc::sample
