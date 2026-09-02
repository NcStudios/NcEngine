#include "gtest/gtest.h"

#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncutility/Hash.h"

#include "graphics2/frontend/subsystem/animation/BoneSnapperSubsystem.h"
#include "graphics2/frontend/subsystem/animation/SkeletalAnimationSubsystem.h"
#include "graphics2/frontend/subsystem/MeshSubsystem.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/Events.h"
#include "ncutility/Hash.h"

const auto g_bonePaths = std::array{
    std::to_string(nc::utility::Fnv1a("1"))
};

const auto g_bonesData = nc::asset::BonesData
{
    .boneMapping = std::unordered_map<std::string, uint32_t>{{"Bone01", 0}},
    .vertexSpaceToBoneSpace = std::vector<nc::asset::VertexSpaceToBoneSpace>
    {
        nc::asset::VertexSpaceToBoneSpace{"Bone01", DirectX::XMMatrixIdentity()}
    },
    .boneSpaceToParentSpace = std::vector<nc::asset::BoneSpaceToParentSpace>
    {
        nc::asset::BoneSpaceToParentSpace{"Bone01", DirectX::XMMatrixIdentity(), 0, 0}
    }
};

const auto g_materialDesc = nc::MaterialDesc{
    .passes = nc::MaterialPassFlag::UniShadow | nc::MaterialPassFlag::PointShadow |nc::MaterialPassFlag::Depth | nc::MaterialPassFlag::Toon | nc::MaterialPassFlag::Normals
};

constexpr auto g_meshView = nc::asset::MeshView{
    .id = nc::utility::Fnv1a("1")
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
            auto& boneStorage = animationSystem.GetStorage();
            boneStorage.LoadBones(g_bonePaths, std::vector<nc::asset::BonesData>{g_bonesData});

            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::SkinnedMesh>(
                entity,
                g_meshView,
                g_materialDesc
            );


            return entity;
        }

        auto AddBoneSnapper(nc::ecs::Ecs& world, nc::Entity targetEntity) -> nc::Entity
        {
            const auto sourceEntity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::BoneSnapper>
            (
                sourceEntity,
                "BoneSocket",
                targetEntity
            );

            return targetEntity;
        }

        static constexpr auto MaxEntities = 20ull;
        nc::SystemEvents systemEvents;
        nc::graphics::SkeletalAnimationSubsystem animationSystem{20};
        nc::graphics::MeshSubsystem meshSubsystem;
        nc::graphics::BoneSnapperSubsystem uut;
};

// TEST_F(BoneSnapperSubsystemTest, Update_targetEntityDoesNotHaveSkinnedMesh_throws)
// {
//     auto world = GetTestWorld();

//     auto sourceEntity = AddStaticMesh(world);
//     auto targetEntity = AddStaticMesh(world);

//     world.Emplace<nc::BoneSnapper>
//     (
//         sourceEntity,
//         "Bone01",
//         targetEntity
//     );

//     GetTestComponentRegistry().CommitPendingChanges();

//     EXPECT_THROW(uut.Update(world), nc::NcError);
// }

TEST_F(BoneSnapperSubsystemTest, Update_targetEntityDoesHaveSkinnedMesh_doesNotThrow)
{
    auto world = GetTestWorld();

    auto sourceEntity = AddStaticMesh(world);
    auto targetEntity = AddSkinnedMesh(world);

    world.Emplace<nc::BoneSnapper>
    (
        sourceEntity,
        "Bone01",
        targetEntity
    );

    GetTestComponentRegistry().CommitPendingChanges();

    EXPECT_NO_THROW(uut.Update(world));
}