#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
#include "ncengine/Events.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Entity.h"
#include "ncengine/ecs/Registry.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "graphics2/frontend/subsystem/MeshRendererSubsystem.h"
#include "graphics2/frontend/subsystem/MeshRendererRenderState.h"

#include <ranges>

DEFINE_ASSET_SERVICE_STUB(meshAssetManager, nc::asset::AssetType::Mesh, nc::asset::MeshView, std::string);
DEFINE_ASSET_SERVICE_STUB(textureAssetManager, nc::asset::AssetType::Texture, nc::asset::TextureView, std::string);

class MeshRendererSubsystemTest : public testing::Test,
                            public EcsFixture
{
    protected:
        static constexpr auto MaxEntities = 20ull;

        nc::Signal<nc::graphics::ToonRenderer&> onAddToonRenderer;
        nc::Signal<nc::Entity> onRemoveToonRenderer;
        nc::SystemEvents events;

        nc::graphics::ToonMaterial dummyMaterial;

        nc::config::GraphicsSettings graphicsSettings;

        nc::graphics::MeshRendererSubsystem uutNoStaticSort; /* No Static Sorting Unit Under Test */
        nc::graphics::MeshRendererSubsystem uutStaticSort;   /* Static Sorting Unit Under Test */


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

        MeshRendererSubsystemTest()
            : EcsFixture{MaxEntities},
              onAddToonRenderer{},
              onRemoveToonRenderer{},
              dummyMaterial{"base", 2, "hatch", 2},
              graphicsSettings{},
              uutNoStaticSort{graphicsSettings},
              uutStaticSort{onAddToonRenderer, onRemoveToonRenderer, events, graphicsSettings}
        {
        }
};

TEST_F(MeshRendererSubsystemTest, BuildState_NoStaticSorting_Succeeds)
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
    auto actualRenderState = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_AllStaticEntities_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_HalfStaticHalfDynamic_Succeeds)
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

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualRenderState.entities.size(), 10);
}

TEST_F(MeshRendererSubsystemTest, BuildState_NoStaticSorting_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    registry.CommitPendingChanges();

    actualRenderState = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualRenderState.entities.size(), 10);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    registry.CommitPendingChanges();

    actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualRenderState.entities.size(), 10);
}

TEST_F(MeshRendererSubsystemTest, BuildState_HalfStaticHalfDynamic_BuildStateAgain_Succeeds)
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

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

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

    actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualRenderState.entities.size(), 10);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_StateNotDirtyRebuild_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddStaticEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

    /* Build again, simulating the next frame, with no dirty static entities. */
    actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

TEST_F(MeshRendererSubsystemTest, BuildState_NoStaticSorting_StateNotDirtyRebuild_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

    /* Build again, simulating the next frame, with no dirty static entities. */
    actualRenderState = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_StaticEntitiesRebuiltEventFires_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        AddEntity(world);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);

    events.rebuildStatics.Emit();

    /* Build again, simulating the next frame, with all dirty static entities. */
    actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

TEST_F(MeshRendererSubsystemTest, BuildState_StaticSorting_EntityToonRendererRemoved_Succeeds)
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

    auto actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 6);
    EXPECT_EQ(actualRenderState.entities.size(), 6);

    world.Remove<ToonRenderer>(entity);

    actualRenderState = uutStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualRenderState.entities.size(), 5);
}

