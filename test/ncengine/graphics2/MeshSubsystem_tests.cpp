#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/Events.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Mesh.h"
#include "graphics2/frontend/subsystem/MeshSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRenderState.h"

#include <array>
#include <ranges>

const auto g_materialDesc = nc::MaterialDesc{
    .passes = nc::MaterialPass::Toon
};

constexpr auto g_meshView = nc::asset::MeshView{
    .id = 42
};

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);

namespace nc
{
MaterialInstance::MaterialInstance(const MaterialDesc&){}
auto MaterialInstance::GetPasses()     const ->       MaterialPasses      { return g_materialDesc.passes;     }
auto MaterialInstance::GetProperties() const -> const MaterialProperties& { return g_materialDesc.properties; }
void MaterialInstance::Release() noexcept {}

namespace time
{
auto DeltaTime() -> float { return 1.0f / 60.0f; }
} // namespace time
} // namespace nc

class MeshSubsystemTest : public testing::Test,
                                  public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;
        nc::SystemEvents systemEvents;
        nc::graphics::MeshSubsystem uut;

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

            return entity;
        }

        MeshSubsystemTest()
            : EcsFixture{MaxEntities},
              uut{systemEvents, MaxEntities, MaxEntities, 1}
        {
            GetTestComponentRegistry().RegisterType<nc::StaticMesh>(MaxEntities);
            GetTestComponentRegistry().RegisterType<nc::SkinnedMesh>(MaxEntities);
        }
};

TEST_F(MeshSubsystemTest, BuildState_BuildsExpectedState)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticMesh(world);
    }

    for (auto i = 0u; i < 3; i++)
    {
        AddSkinnedMesh(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uut.BuildState(world);
    const auto& actualTransformState = actualRenderState.transformData;
    EXPECT_EQ(8, actualTransformState.instances.size());
    ASSERT_EQ(1, actualTransformState.dirtyRanges.size());
    EXPECT_EQ(0, actualTransformState.dirtyRanges[0].offset);
    EXPECT_EQ(8, actualTransformState.dirtyRanges[0].count);

    const auto& actualStaticInstanceState = actualRenderState.staticMeshInstanceData;
    EXPECT_EQ(5, actualStaticInstanceState.instances.size());
    ASSERT_EQ(1, actualStaticInstanceState.dirtyRanges.size());
    EXPECT_EQ(0, actualStaticInstanceState.dirtyRanges[0].offset);
    EXPECT_EQ(5, actualStaticInstanceState.dirtyRanges[0].count);

    const auto& actualSkinnedInstanceState = actualRenderState.skinnedMeshInstanceData;
    EXPECT_EQ(3, actualSkinnedInstanceState.instances.size());
    ASSERT_EQ(1, actualSkinnedInstanceState.dirtyRanges.size());
    EXPECT_EQ(0, actualSkinnedInstanceState.dirtyRanges[0].offset);
    EXPECT_EQ(3, actualSkinnedInstanceState.dirtyRanges[0].count);

    const auto& actualStaticPassBatches = actualRenderState.staticMeshBatches;
    ASSERT_EQ(1, actualStaticPassBatches.size());
    const auto& actualStaticBatches = actualStaticPassBatches.at(0);
    EXPECT_EQ(1, actualStaticBatches.size());
    const auto& actualStaticBatch = actualStaticBatches.at(0);
    EXPECT_EQ(0, actualStaticBatch.firstInstance);
    EXPECT_EQ(5, actualStaticBatch.instanceCount);

    const auto& actualSkinnedPassBatches = actualRenderState.skinnedMeshBatches;
    ASSERT_EQ(1, actualSkinnedPassBatches.size());
    const auto& actualSkinnedBatches = actualSkinnedPassBatches.at(0);
    EXPECT_EQ(1, actualSkinnedBatches.size());
    const auto& actualSkinnedBatch = actualSkinnedBatches.at(0);
    EXPECT_EQ(0, actualSkinnedBatch.firstInstance);
    EXPECT_EQ(3, actualSkinnedBatch.instanceCount);

    registry.Clear();
}

TEST_F(MeshSubsystemTest, OnRemoveMesh_UntracksObject)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    const auto first = AddStaticMesh(world);
    AddStaticMesh(world);
    registry.CommitPendingChanges();
    uut.BuildState(world); // discard - just updating internal tracking

    world.Remove<nc::Entity>(first);
    registry.CommitPendingChanges();

    // Only the remaining transform needs to be updated
    auto actualRenderState = uut.BuildState(world);
    const auto& actualTransformState = actualRenderState.transformData;
    EXPECT_EQ(2, actualTransformState.instances.size());
    ASSERT_EQ(1, actualTransformState.dirtyRanges.size());
    EXPECT_EQ(1, actualTransformState.dirtyRanges[0].offset);
    EXPECT_EQ(1, actualTransformState.dirtyRanges[0].count);

    // Whole instance buffer needs to be updated since we removed the first item
    const auto& actualInstanceState = actualRenderState.staticMeshInstanceData;
    EXPECT_EQ(2, actualInstanceState.instances.size());
    ASSERT_EQ(1, actualInstanceState.dirtyRanges.size());
    EXPECT_EQ(0, actualInstanceState.dirtyRanges[0].offset);
    EXPECT_EQ(2, actualInstanceState.dirtyRanges[0].count);

    // Batch reports only one instance
    const auto& actualPassState = actualRenderState.staticMeshBatches;
    ASSERT_EQ(1, actualPassState.size());
    const auto& actualBatches = actualPassState.at(0);
    EXPECT_EQ(1, actualBatches.size());
    const auto& actualBatch = actualBatches.at(0);
    EXPECT_EQ(0, actualBatch.firstInstance);
    EXPECT_EQ(1, actualBatch.instanceCount);

    registry.Clear();
}

TEST_F(MeshSubsystemTest, UpdateMesh_UsingSetMesh_PatchesTrackedState)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    AddStaticMesh(world);
    AddSkinnedMesh(world);
    const auto secondStatic = AddStaticMesh(world);
    const auto secondSkinned = AddSkinnedMesh(world);
    registry.CommitPendingChanges();
    uut.BuildState(world); // discard - just updating internal tracking

    world.Get<nc::StaticMesh>(secondStatic).SetMesh(nc::asset::MeshView{.id = 100});
    world.Get<nc::SkinnedMesh>(secondSkinned).SetMesh(nc::asset::MeshView{.id = 100});

    // Split into two batches. Second batch should be offset at index 2, leaving a free space in the first batch.
    // Same scenario for both static/skinned types.
    auto actualRenderState = uut.BuildState(world);
    auto verifyBatches = [](const auto& actualPassState)
    {
        ASSERT_EQ(1, actualPassState.size());
        const auto& actualBatches = actualPassState.at(0);
        EXPECT_EQ(2, actualBatches.size());
        const auto& actualBatch1 = actualBatches.at(0);
        const auto& actualBatch2 = actualBatches.at(1);
        EXPECT_EQ(0, actualBatch1.firstInstance);
        EXPECT_EQ(1, actualBatch1.instanceCount);
        EXPECT_EQ(2, actualBatch2.firstInstance);
        EXPECT_EQ(1, actualBatch2.instanceCount);
    };

    verifyBatches(actualRenderState.staticMeshBatches);
    verifyBatches(actualRenderState.skinnedMeshBatches);

    registry.Clear();
}

TEST_F(MeshSubsystemTest, MeshRendererUpdateMaterial_Succeeds)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    const auto staticMesh = AddStaticMesh(world);
    const auto skinnedMesh = AddSkinnedMesh(world);
    registry.CommitPendingChanges();
    uut.BuildState(world); // discard - just updating internal tracking

    // Only 1 pass is implemented currently, so we can't actually assign new passes/move to a new batch.
    // Eventually, we should make this test more interesting.
    EXPECT_NO_THROW(world.Get<nc::StaticMesh>(staticMesh).SetMaterial(g_materialDesc));
    EXPECT_NO_THROW(world.Get<nc::SkinnedMesh>(skinnedMesh).SetMaterial(g_materialDesc));

    registry.Clear();
}
