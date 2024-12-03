#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/Events.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/MeshRenderer2.h"
#include "graphics2/frontend/subsystem/MeshRendererSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include <array>
#include <ranges>

const auto g_materialDesc = nc::MaterialDesc{
    .passes = nc::MaterialPassFlag::Toon
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
} // namespace nc

class MeshRendererSubsystemTest : public testing::Test,
                                  public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;
        nc::SystemEvents systemEvents;
        nc::graphics::MeshRendererSubsystem uut;

        auto AddEntity(nc::ecs::Ecs& world) -> nc::Entity
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<nc::MeshRenderer2>(
                entity,
                g_meshView,
                g_materialDesc
            );

            return entity;
        }

        MeshRendererSubsystemTest()
            : EcsFixture{MaxEntities},
              uut{systemEvents, MaxEntities, MaxEntities, 1}
        {
            GetTestComponentRegistry().RegisterType<nc::MeshRenderer2>(MaxEntities);
        }
};

TEST_F(MeshRendererSubsystemTest, BuildState_BuildsExpectedState)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uut.BuildState(world);
    const auto& actualTransformState = actualRenderState.transformData;
    EXPECT_EQ(5, actualTransformState.instances.size());
    ASSERT_EQ(1, actualTransformState.dirtyRanges.size());
    EXPECT_EQ(0, actualTransformState.dirtyRanges[0].offset);
    EXPECT_EQ(5, actualTransformState.dirtyRanges[0].count);

    const auto& actualInstanceState = actualRenderState.instanceData;
    EXPECT_EQ(5, actualInstanceState.instances.size());
    ASSERT_EQ(1, actualInstanceState.dirtyRanges.size());
    EXPECT_EQ(0, actualInstanceState.dirtyRanges[0].offset);
    EXPECT_EQ(5, actualInstanceState.dirtyRanges[0].count);

    const auto& actualPassState = actualRenderState.passBatches;
    ASSERT_EQ(1, actualPassState.size());
    const auto& actualBatches = actualPassState.at(0);
    EXPECT_EQ(1, actualBatches.size());
    const auto& actualBatch = actualBatches.at(0);
    EXPECT_EQ(0, actualBatch.firstInstance);
    EXPECT_EQ(5, actualBatch.instanceCount);

    registry.Clear();
}

TEST_F(MeshRendererSubsystemTest, OnRemoveMeshRenderer_UntracksObject)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    const auto first = AddEntity(world);
    AddEntity(world);
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
    const auto& actualInstanceState = actualRenderState.instanceData;
    EXPECT_EQ(2, actualInstanceState.instances.size());
    ASSERT_EQ(1, actualInstanceState.dirtyRanges.size());
    EXPECT_EQ(0, actualInstanceState.dirtyRanges[0].offset);
    EXPECT_EQ(2, actualInstanceState.dirtyRanges[0].count);

    // Batch reports only one instance
    const auto& actualPassState = actualRenderState.passBatches;
    ASSERT_EQ(1, actualPassState.size());
    const auto& actualBatches = actualPassState.at(0);
    EXPECT_EQ(1, actualBatches.size());
    const auto& actualBatch = actualBatches.at(0);
    EXPECT_EQ(0, actualBatch.firstInstance);
    EXPECT_EQ(1, actualBatch.instanceCount);

    registry.Clear();
}

TEST_F(MeshRendererSubsystemTest, MeshRendererUpdateMesh_PatchesTrackedState)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    AddEntity(world);
    const auto second = AddEntity(world);
    registry.CommitPendingChanges();
    uut.BuildState(world); // discard - just updating internal tracking

    world.Get<nc::MeshRenderer2>(second).SetMesh(nc::asset::MeshView{.id = 100});

    // Split into two batches. Second batch should be offset at index 2, leaving a free space in the first batch.
    auto actualRenderState = uut.BuildState(world);
    const auto& actualPassState = actualRenderState.passBatches;
    ASSERT_EQ(1, actualPassState.size());
    const auto& actualBatches = actualPassState.at(0);
    EXPECT_EQ(2, actualBatches.size());
    const auto& actualBatch1 = actualBatches.at(0);
    const auto& actualBatch2 = actualBatches.at(1);
    EXPECT_EQ(0, actualBatch1.firstInstance);
    EXPECT_EQ(1, actualBatch1.instanceCount);
    EXPECT_EQ(2, actualBatch2.firstInstance);
    EXPECT_EQ(1, actualBatch2.instanceCount);

    registry.Clear();
}

TEST_F(MeshRendererSubsystemTest, MeshRendererUpdateMaterial_Succeeds)
{
    auto world = GetTestWorld();
    auto& registry = GetTestComponentRegistry();
    const auto entity = AddEntity(world);
    registry.CommitPendingChanges();
    uut.BuildState(world); // discard - just updating internal tracking

    // Only 1 pass is implemented currently, so we can't actually assign new passes/move to a new batch.
    // Eventually, we should make this test more interesting.
    EXPECT_NO_THROW(world.Get<nc::MeshRenderer2>(entity).SetMaterial(g_materialDesc));

    registry.Clear();
}
