#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/Events.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "graphics2/frontend/subsystem/ObjectSubsystem.h"
#include "graphics2/frontend/subsystem/ObjectRenderState.h"

#include <ranges>

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::asset::TextureView, std::string);

class ObjectSubsystemTest : public testing::Test,
                            public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;

        nc::Signal<nc::graphics::ToonRenderer&> onAddToonRenderer;
        nc::Signal<nc::Entity> onRemoveToonRenderer;
        nc::SystemEvents events;

        nc::graphics::ToonMaterial dummyMaterial;

        nc::graphics::ObjectSubsystem uutNoStaticSort; /* No Static Sorting Unit Under Test */
        nc::graphics::ObjectSubsystem uutStaticSort;   /* Static Sorting Unit Under Test */

        void AddStaticEntity(nc::ecs::Ecs& world) 
        {
            auto flags = nc::Entity::Flags::Static;
            const auto entity = world.Emplace<nc::Entity>({.flags = flags});
            auto& toonRenderer = world.Emplace<nc::graphics::ToonRenderer>(entity, "mesh.nca", dummyMaterial);
            onAddToonRenderer.Emit(toonRenderer); /* Simulate registry OnAdd Event */
        }

        void AddEntity(nc::ecs::Ecs& world) 
        {
            const auto entity = world.Emplace<nc::Entity>({});
            auto& toonRenderer = world.Emplace<nc::graphics::ToonRenderer>(entity, "mesh.nca", dummyMaterial);
            onAddToonRenderer.Emit(toonRenderer); /* Simulate registry OnAdd Event */
        }

        ObjectSubsystemTest()
            : EcsFixture{MaxEntities},
              onAddToonRenderer{},
              onRemoveToonRenderer{},
              dummyMaterial{"base", 2, "hatch", 2},
              uutNoStaticSort{},
              uutStaticSort{onAddToonRenderer, onRemoveToonRenderer, events}
        {
        }
};

TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    // Curious why this deduces to a && ref?
    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_AllStaticEntities_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_HalfStaticHalfDynamic_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntities.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    registry.CommitPendingChanges();

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntitiesRebuild.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    registry.CommitPendingChanges();

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntitiesRebuild.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_HalfStaticHalfDynamic_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        if (i % 2 == 1)
        {
            AddStaticEntity(world);
        }
        else
        {
            AddEntity(world);
        }
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        if (i % 2 == 0)
        {
            AddStaticEntity(world);
        }
        else
        {
            AddEntity(world);
        }
    }

    registry.CommitPendingChanges();

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntitiesRebuild.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_StateNotDirtyRebuild_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    /* Build again, simulating the next frame, with no dirty static entities. */
    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntitiesRebuild.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_StateNotDirtyRebuild_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    /* Build again, simulating the next frame, with no dirty static entities. */
    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntitiesRebuild.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_StaticEntitiesRebuiltEventFires_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    events.rebuildStatics.Emit();

    /* Build again, simulating the next frame, with all dirty static entities. */
    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntitiesRebuild.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_EntityToonRendererRemoved_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    /* Add entity to the beginning */
    const auto entity = world.Emplace<nc::Entity>({});
    auto& toonRenderer = world.Emplace<nc::graphics::ToonRenderer>(entity, "mesh.nca", dummyMaterial);
    onAddToonRenderer.Emit(toonRenderer); /* Simulate registry OnAdd Event */

    /* Add five more entities */
    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 6);
    EXPECT_EQ(actualEntities.size(), 6);

    world.Remove<ToonRenderer>(entity);

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntitiesRebuild.size(), 5);
}

