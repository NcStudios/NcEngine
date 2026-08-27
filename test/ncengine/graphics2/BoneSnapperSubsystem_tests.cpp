#include "gtest/gtest.h"

#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/Events.h"
#include "graphics2/frontend/subsystem/animation/BoneSnapperSubsystem.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationSubsystem.h"
#include "graphics2/frontend/subsystem/MeshSubsystem.h"
#include "ncengine/ecs/Ecs.h"
#include "ncutility/Hash.h"

auto MakeAnimation() -> nc::asset::SkeletalAnimation
{
    using namespace nc;
    using namespace nc::asset;
    auto positionFrame = PositionFrame{0.0f, Vector3{1.0f, 1.0f, 1.0f}};
    auto positionFrame2 = PositionFrame{1.0f, Vector3{2.0f, 2.0f, 2.0f}};
    auto positionFrame3 = PositionFrame{2.0f, Vector3{3.0f, 3.0f, 3.0f}};
    auto positionFrames = std::vector<PositionFrame>{positionFrame, positionFrame2, positionFrame3};

    auto rotationFrame = RotationFrame{0.0f, Quaternion::Identity()};
    auto rotationFrame2 = RotationFrame{1.0f, Quaternion::Identity()};
    auto rotationFrame3 = RotationFrame{2.0f, Quaternion::Identity()};
    auto rotationFrames = std::vector<RotationFrame>{rotationFrame, rotationFrame2, rotationFrame3};

    auto scaleFrame = ScaleFrame{0.0f, Vector3{1.0f, 1.0f, 1.0f}};
    auto scaleFrame2 = ScaleFrame{1.0f, Vector3{2.0f, 2.0f, 2.0f}};
    auto scaleFrame3 = ScaleFrame{2.0f, Vector3{3.0f, 3.0f, 3.0f}};
    auto scaleFrames = std::vector<ScaleFrame>{scaleFrame, scaleFrame2, scaleFrame3};

    auto frames = SkeletalAnimationFrames{
        .positionFrames = std::move(positionFrames),
        .rotationFrames = std::move(rotationFrames),
        .scaleFrames = std::move(scaleFrames),
    };

    auto framesPerBones = std::unordered_map<std::string, SkeletalAnimationFrames>{};
    framesPerBones.emplace("BoneSocket", frames);
    framesPerBones.emplace("Bone1", frames);
    framesPerBones.emplace("Bone3", SkeletalAnimationFrames{});

    return SkeletalAnimation{
        .name = "test",
        .durationInTicks = 1,
        .ticksPerSecond = 1,
        .framesPerBone = std::move(framesPerBones)
    };
}

// auto MakeBonesData() -> nc::asset::BonesData
// {
//     using namespace nc::asset;
//     auto bonesData = BonesData{
//         .boneMapping = std::unordered_map<std::string, uint32_t>{},
//         .vertexSpaceToBoneSpace = std::vector<VertexSpaceToBoneSpace>(0),
//         .boneSpaceToParentSpace = std::vector<BoneSpaceToParentSpace>(0)
//     };

//     const auto matrix1 = DirectX::XMMATRIX{
//         1, 0, 0, 0,
//         0, 1, 0, 0,
//         0, 0, 1, 0,
//         0, 0, 0, 1
//     };

//     const auto matrix2 = DirectX::XMMATRIX{
//         1, 0, 0, 0,
//         0, 1, 0, 0,
//         0, 0, 1, 0,
//         1, 0, 0, 1
//     };

//     bonesData.vertexSpaceToBoneSpace.emplace_back(std::string("BoneSocket"), matrix1);
//     bonesData.vertexSpaceToBoneSpace.emplace_back(std::string("Bone1"), matrix1);
//     bonesData.boneSpaceToParentSpace.emplace_back(std::string("BoneSocket"), matrix2, 0u, 0u);
//     bonesData.boneSpaceToParentSpace.emplace_back(std::string("Bone1"), matrix2, 0u, 0u);
//     bonesData.boneSpaceToParentSpace.emplace_back(std::string("Bone2"), matrix2, 0u, 0u);
//     bonesData.boneMapping.emplace("BoneSocket", 0);
//     bonesData.boneMapping.emplace("Bone1", 1);
//     bonesData.boneMapping.emplace("Bone2", 2);
//     return bonesData;
// }

const auto g_animation = MakeAnimation();
// const auto g_bonesData = MakeBonesData();

const auto g_animPaths = std::array{
    std::string{"test.nca"}
};

const auto g_animId1 = nc::utility::Fnv1a(g_animPaths[0]);

const auto g_materialDesc = nc::MaterialDesc{
    .passes = nc::MaterialPassFlag::UniShadow | nc::MaterialPassFlag::PointShadow |nc::MaterialPassFlag::Depth | nc::MaterialPassFlag::Toon | nc::MaterialPassFlag::Normals
};

constexpr auto g_meshView = nc::asset::MeshView{
    .id = 42
};

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc&){}
auto MaterialInstance::GetPasses()     const ->       MaterialPassFlags   { return g_materialDesc.passes;     }
auto MaterialInstance::GetProperties() const -> const MaterialProperties& { return g_materialDesc.properties; }
void MaterialInstance::Release() noexcept {}

namespace time
{
auto DeltaTime() -> float { return 1.0f / 60.0f; }
} // namespace time
} // namespace nc

class BoneSnapperSubsystemTest : public testing::Test, public EcsFixture
{
    protected:
        BoneSnapperSubsystemTest()
            : EcsFixture{MaxEntities},
              animationSystem{MaxEntities},
              meshSubsystem{animationSystem, systemEvents,MaxEntities,MaxEntities,1},
              uut{animationSystem}
        {
            GetTestComponentRegistry().RegisterType<nc::StaticMesh>(MaxEntities);
            GetTestComponentRegistry().RegisterType<nc::SkinnedMesh>(MaxEntities);
            GetTestComponentRegistry().RegisterType<nc::BoneSnapper>(MaxEntities);
        }
        
        auto AddStaticMesh(nc::ecs::Ecs& world) -> nc::Entity
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::StaticMesh>(
                entity,
                g_meshView,
                g_materialDesc
            );

            return entity;
        }

        auto AddSkinnedMesh(nc::ecs::Ecs& world) -> nc::Entity
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::SkinnedMesh>(
                entity,
                g_meshView,
                g_materialDesc
            );

            auto& boneStorage = animationSystem.GetStorage();
            boneStorage.LoadAnimations(g_animPaths, std::vector<nc::asset::SkeletalAnimation>{g_animation});

            return entity;
        }

        auto AddBoneSnapper(nc::ecs::Ecs& world, nc::Entity target) -> nc::Entity
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::BoneSnapper>
            (
                entity,
                "BoneSocket",
                target
            );

            return entity;
        }

        static constexpr auto MaxEntities = 20ull;
        nc::SystemEvents systemEvents;
        nc::graphics::SkeletalAnimationSubsystem animationSystem{20};
        nc::graphics::MeshSubsystem meshSubsystem;
        nc::graphics::BoneSnapperSubsystem uut;
};


TEST_F(BoneSnapperSubsystemTest, Update_entityDoesNotHaveSkinnedMesh_Skipped)
{
    auto world = GetTestWorld();
    AddStaticMesh(world);
    AddSkinnedMesh(world);
    const auto entity = world.Emplace<nc::Entity>({});
    auto entityWithBoneSnapper = AddBoneSnapper(world, entity);
    uut.Update(world);
}