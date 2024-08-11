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

void GraphicsTest::Load(ecs::Ecs world, ModuleProvider modules)
{
    m_sampleUI->SetWidgetCallback(nullptr);

    std::vector<std::string> textures
    {
        "ogre/BaseColor.nca",
        "ogre/Roughness.nca",
        "ogre/Metallic.nca",
        "cave/BaseColor.nca",
        "cave/Roughness.nca",
        "cave/Metallic.nca",
        "cave_ceiling/BaseColor.nca",
        "cave_ceiling/Roughness.nca",
        "cave_ceiling/Metallic.nca",
        "skeleton/BaseColor.nca",
        "skeleton/Roughness.nca",
        "skeleton/Metallic.nca",
        "denim/BaseColor.nca",
        "denim/Roughness.nca",
        "denim/Metallic.nca",
        "marble/BaseColor.nca",
        "marble/Roughness.nca",
        "marble/Metallic.nca",
        "metal/BaseColor.nca",
        "metal/Roughness.nca",
        "metal/Metallic.nca",
        "plastic/BaseColor.nca",
        "plastic/Roughness.nca",
        "plastic/Metallic.nca",
        "silver/BaseColor.nca",
        "silver/Roughness.nca",
        "silver/Metallic.nca",
        "stone/BaseColor.nca",
        "stone/Roughness.nca",
        "stone/Metallic.nca",
        "tile/BaseColor.nca",
        "tile/Roughness.nca",
        "tile/Metallic.nca",
        "wood/BaseColor.nca",
        "wood/Roughness.nca",
        "wood/Metallic.nca"
    };

    std::vector<std::string> normals
    {
        "ogre/Normal.nca",
        "cave/Normal.nca",
        "cave_ceiling/Normal.nca",
        "skeleton/Normal.nca",
        "denim/Normal.nca",
        "marble/Normal.nca",
        "metal/Normal.nca",
        "plastic/Normal.nca",
        "silver/Normal.nca",
        "stone/Normal.nca",
        "tile/Normal.nca",
        "wood/Normal.nca"
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
        "ogre/idle.nca",
        "ogre/attack.nca",
        "skeleton/idle.nca",
        "skeleton/walk_right.nca",
        "skeleton/walk_back.nca",
        "skeleton/walk_forward.nca",
        "skeleton/walk_left.nca",
        "skeleton/jump.nca",
    };

    asset::LoadSkeletalAnimationAssets(animations);
    asset::LoadTextureAssets(textures, false, asset::AssetFlags::TextureTypeImage);
    asset::LoadTextureAssets(normals, false, asset::AssetFlags::TextureTypeNormalMap);
    asset::LoadMeshAssets(meshes);
    asset::LoadCubeMapAssets(cubemaps);

    auto ogreMaterial = graphics::PbrMaterial{
        .baseColor  = "ogre/BaseColor.nca",
        .normal     = "ogre/Normal.nca",
        .roughness  = "ogre/Roughness.nca",
        .metallic   = "ogre/Metallic.nca"
    };

    auto skeletonMaterial = graphics::PbrMaterial{
        .baseColor  = "skeleton/BaseColor.nca",
        .normal     = "skeleton/Normal.nca",
        .roughness  = "skeleton/Roughness.nca",
        .metallic   = "skeleton/Metallic.nca"
    };

    auto caveMaterial = graphics::PbrMaterial{
        .baseColor  = "cave/BaseColor.nca",
        .normal     = "cave/Normal.nca",
        .roughness  = "cave/Roughness.nca",
        .metallic   = "cave/Metallic.nca"
    };

    auto caveCeilingMaterial = graphics::PbrMaterial{
        .baseColor  = "cave_ceiling/BaseColor.nca",
        .normal     = "cave_ceiling/Normal.nca",
        .roughness  = "cave_ceiling/Roughness.nca",
        .metallic   = "cave_ceiling/Metallic.nca"
    };

    auto marbleMaterial = graphics::PbrMaterial{
        .baseColor  = "marble/BaseColor.nca",
        .normal     = "marble/Normal.nca",
        .roughness  = "marble/Roughness.nca",
        .metallic   = "marble/Metallic.nca"
    };

    auto metalMaterial = graphics::PbrMaterial{
        .baseColor  = "metal/BaseColor.nca",
        .normal     = "metal/Normal.nca",
        .roughness  = "metal/Roughness.nca",
        .metallic   = "metal/Metallic.nca"
    };

    auto denimMaterial = graphics::PbrMaterial{
        .baseColor  = "denim/BaseColor.nca",
        .normal     = "denim/Normal.nca",
        .roughness  = "denim/Roughness.nca",
        .metallic   = "denim/Metallic.nca"
    };

    auto plasticMaterial = graphics::PbrMaterial{
        .baseColor  = "plastic/BaseColor.nca",
        .normal     = "plastic/Normal.nca",
        .roughness  = "plastic/Roughness.nca",
        .metallic   = "plastic/Metallic.nca"
    };

    auto stoneMaterial = graphics::PbrMaterial{
        .baseColor  = "stone/BaseColor.nca",
        .normal     = "stone/Normal.nca",
        .roughness  = "stone/Roughness.nca",
        .metallic   = "stone/Metallic.nca"
    };

    auto tileMaterial = graphics::PbrMaterial{
        .baseColor  = "tile/BaseColor.nca",
        .normal     = "tile/Normal.nca",
        .roughness  = "tile/Roughness.nca",
        .metallic   = "tile/Metallic.nca"
    };

    modules.Get<graphics::NcGraphics>()->SetSkybox("night_sky.nca");
    // auto ncGraphics = modules.Get<graphics::NcGraphics>();

    // Lights
    auto lvHandle = world.Emplace<Entity>({.position = Vector3{0.0f, 0.0f, 0.0f}, .tag = "Point Light 1"});
    world.Emplace<graphics::PointLight>(lvHandle, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.946f, 0.671f, 0.278f), 26.6f);
    // auto lv2Handle = world.Emplace<Entity>({.position = Vector3{6.5f, 9.0f, 9.6f}, .tag = "Point Light 2"});
    // world.Emplace<graphics::PointLight>(lv2Handle, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.723f, 0.608f), 13.4f);
    // auto lv3Handle = world.Emplace<Entity>({.position = Vector3{4.5f, 6.0f, -8.4f}, .tag = "Point Light 3"});
    // world.Emplace<graphics::PointLight>(lv3Handle, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 7.3f);

    // Ogre
    auto ogre = world.Emplace<Entity>({
        .position = Vector3{-5.0f, 0.0f, 12.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.0f, 0.0f),
        .scale = Vector3{3.0f, 3.0f, 3.0f},
        .tag = "ogre"
    });
    world.Emplace<graphics::MeshRenderer>(ogre, "ogre.nca", ogreMaterial);
    auto& collider = world.Emplace<physics::Collider>(ogre, physics::SphereProperties{}, false);
    world.Emplace<physics::PhysicsBody>(
        ogre,
        world.Get<Transform>(ogre),
        collider,
        physics::PhysicsProperties{
            .mass = 0.0f,
            .isKinematic = true
        }
    );

    // Ogre Animation
    {
        using namespace graphics;
        auto& ogreAnimator = world.Emplace<SkeletalAnimator>(ogre, "ogre.nca", "ogre/idle.nca");
        auto stopState = ogreAnimator.AddState(anim::Stop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);}
        });

        ogreAnimator.AddState(anim::Loop
        {
            .enterFrom = stopState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .animUid = "ogre/idle.nca",
            .exitWhen = [](){ return input::KeyDown(input::KeyCode::One);},
            .exitTo = stopState
        });
    }

    // Skeleton
    auto skeleton = world.Emplace<Entity>({
        .position = Vector3{5.3f, 0.0f, -6.4f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.5f, 0.0f),
        .scale = Vector3{2.0f, 2.0f, 2.0f},
        .tag = "skeleton"
    });

    // world.Emplace<FrameLogic>(lvHandle, [ncGraphics, isEnabled = false](nc::Entity, Registry*, float) mutable
    // {
    //     if (KeyDown(input::KeyCode::H))
    //     {
    //         isEnabled = !isEnabled;
    //         ncGraphics->EnableShadowTest(isEnabled);
    //     }
    // });


    world.Emplace<graphics::MeshRenderer>(skeleton, "skeleton.nca", skeletonMaterial);
    world.Emplace<FrameLogic>(skeleton, WasdBasedMovement);
    world.Emplace<physics::Collider>(skeleton, physics::SphereProperties{}, true);
    world.Emplace<CollisionLogic>(skeleton, nullptr, nullptr,
    [](Entity, Entity other, Registry* reg)
    {
        auto ogreAnim = reg->Get<graphics::SkeletalAnimator>(other);
        ogreAnim->PlayOnceImmediate("ogre/attack.nca", graphics::anim::RootState);
        if(auto* tag = reg->Get<Tag>(other); tag)
        {
            GameLog::Log(std::string{"Collision Enter: "} + tag->value.c_str());
        }
    }, nullptr);

    // Skeleton Animation
    {
        using namespace graphics;
        auto& skelAnim = world.Emplace<SkeletalAnimator>(skeleton, "skeleton.nca", "skeleton/idle.nca");
        skelAnim.AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::W);},
            .animUid = "skeleton/walk_forward.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::W);}
        });

        skelAnim.AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::A);},
            .animUid = "skeleton/walk_left.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::A);}
        });

        skelAnim.AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::S);},
            .animUid = "skeleton/walk_back.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::S);}
        });

        skelAnim.AddState(anim::Loop
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyHeld(input::KeyCode::D);},
            .animUid = "skeleton/walk_right.nca",
            .exitWhen = [](){ return input::KeyUp(input::KeyCode::D);}
        });

        skelAnim.AddState(anim::PlayOnce
        {
            .enterFrom = anim::RootState,
            .enterWhen = [](){ return input::KeyDown(input::KeyCode::Space);},
            .animUid = "skeleton/jump.nca"
        });
    }

    // Cave
    auto cave_floor = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.5708f, 0.0f),
        .scale = Vector3{1.5f, 1.5f, 1.5f},
        .tag = "cave_floor"
    });
    world.Emplace<graphics::MeshRenderer>(cave_floor, "cave.nca", caveMaterial);

    auto cave_ceiling = world.Emplace<Entity>({
        .position = Vector3{0.0f, 0.0f, 0.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 1.5708f, 0.0f),
        .scale = Vector3{1.5f, 1.5f, 1.5f},
        .tag = "cave_ceiling"
    });
    world.Emplace<graphics::MeshRenderer>(cave_ceiling, "cave_ceiling.nca", caveCeilingMaterial);

    auto marbleSphere = world.Emplace<Entity>({
        .position = Vector3{6.0f, 3.0f, 13.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "marble"
    });
    world.Emplace<graphics::MeshRenderer>(marbleSphere, "DefaultSphere.nca", marbleMaterial);

    auto metalSphere = world.Emplace<Entity>({
        .position = Vector3{8.0f, 3.0f, 12.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "metal"
    });
    world.Emplace<graphics::MeshRenderer>(metalSphere, "DefaultSphere.nca", metalMaterial);

    auto denimSphere = world.Emplace<Entity>({
        .position = Vector3{9.0f, 3.0f, 10.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "denim"
    });
    world.Emplace<graphics::MeshRenderer>(denimSphere, "DefaultSphere.nca", denimMaterial);

    auto plasticSphere = world.Emplace<Entity>({
        .position = Vector3{6.0f, 5.0f, 13.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "plastic"
    });
    world.Emplace<graphics::MeshRenderer>(plasticSphere, "DefaultSphere.nca", plasticMaterial);

    auto stoneSphere = world.Emplace<Entity>({
        .position = Vector3{8.0f, 5.0f, 12.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "stone"
    });
    world.Emplace<graphics::MeshRenderer>(stoneSphere, "DefaultSphere.nca", stoneMaterial);

    auto tileSphere = world.Emplace<Entity>({
        .position = Vector3{9.0f, 5.0f, 10.0f},
        .rotation = Quaternion::FromEulerAngles(0.0f, 0.0f, 0.0f),
        .scale = Vector3{1.0f, 1.0f, 1.0f},
        .tag = "tile"
    });
    world.Emplace<graphics::MeshRenderer>(tileSphere, "DefaultSphere.nca", tileMaterial);

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
