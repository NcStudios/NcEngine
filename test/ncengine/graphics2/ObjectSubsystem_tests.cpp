#include "gtest/gtest.h"
#include "../AssetServiceStub.h"
#include "../EcsFixture.inl"
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

        nc::graphics::ToonMaterial dummyMaterial;

        nc::graphics::ObjectSubsystem uutNoStaticSort; // No Static Sorting Unit Under Test
        nc::graphics::ObjectSubsystem uutStaticSort;   // Static Sorting Unit Under Test

        ObjectSubsystemTest()
            : EcsFixture{MaxEntities},
              onAddToonRenderer{},
              onRemoveToonRenderer{},
              dummyMaterial{"base", 2, "hatch", 2},
              uutNoStaticSort{},
              uutStaticSort{onAddToonRenderer, onRemoveToonRenderer}
        {
        }
};

TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        const auto entity = world.Emplace<nc::Entity>({});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
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
        auto flags = nc::Entity::Flags::Static;
        const auto entity = world.Emplace<nc::Entity>({.flags = flags});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_HalfStaticHalfDynamic_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        auto flags = nc::Entity::Flags::Static;
        const auto entity = world.Emplace<nc::Entity>({.flags = flags});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    for (auto i = 0u; i < 5; i++)
    {
        const auto entity = world.Emplace<nc::Entity>({});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntities.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_NoStaticSorting_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        const auto entity = world.Emplace<nc::Entity>({});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        const auto entity = world.Emplace<nc::Entity>({});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
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
        auto flags = nc::Entity::Flags::Static;
        const auto entity = world.Emplace<nc::Entity>({.flags = flags});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        auto flags = nc::Entity::Flags::Static;
        const auto entity = world.Emplace<nc::Entity>({.flags = flags});
        world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        world.Get<nc::Transform>(entity).Translate(nc::Vector3{i * 5.0f, 0.0f, 0});
    }

    registry.CommitPendingChanges();

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntitiesRebuild.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_HalfStaticHalfDynamic_BuildStateAgain_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        auto flags = nc::Entity::Flags::Static;
        if (i % 2 == 1)
        {
          const auto entity = world.Emplace<nc::Entity>({.flags = flags});
          world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        }
        else
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        }
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    for (auto i = 0u; i < 5; i++)
    {
        auto flags = nc::Entity::Flags::Static;
        if (i % 2 == 0)
        {
          const auto entity = world.Emplace<nc::Entity>({.flags = flags});
          world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        }
        else
        {
            const auto entity = world.Emplace<nc::Entity>({});
            world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
        }
    }

    registry.CommitPendingChanges();

    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 10);
    EXPECT_EQ(actualEntitiesRebuild.size(), 10);
}

TEST_F(ObjectSubsystemTest, BuildState_StaticSorting_StateNotDirty_Succeeds)
{
    using namespace nc::graphics;

    auto world = GetTestWorld();

    for (auto i = 0u; i < 5; i++)
    {
        auto flags = nc::Entity::Flags::Static;
          const auto entity = world.Emplace<nc::Entity>({.flags = flags});
          world.Emplace<ToonRenderer>(entity, "mesh.nca", dummyMaterial);
    }

    auto& registry = GetTestComponentRegistry();
    registry.CommitPendingChanges();

    auto [actualRenderState, actualEntities] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderState.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntities.size(), 5);

    //  Build again, simulating the next frame, with no dirty static entities.
    auto [actualRenderStateRebuild, actualEntitiesRebuild] = uutNoStaticSort.BuildState(world);
    EXPECT_EQ(actualRenderStateRebuild.modelMatrices.size(), 5);
    EXPECT_EQ(actualEntitiesRebuild.size(), 5);
}
