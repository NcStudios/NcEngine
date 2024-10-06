#include "gtest/gtest.h"
#include "ncengine/ecs/ComponentRegistry.h"

struct S1 {};

struct S2 : public nc::ComponentBase
{
    S2(nc::Entity e) : nc::ComponentBase{e} {}
};

TEST(ComponentRegistryTests, Constructor_enforcesSingleInstnance)
{
    {
        auto old = nc::ecs::ComponentRegistry{10};
        old.RegisterType<S1>(1);
        ASSERT_TRUE(old.IsTypeRegistered<S1>());
        EXPECT_THROW(nc::ecs::ComponentRegistry{10}, nc::NcError); // only 1 instance allowed
    }

    auto uut = nc::ecs::ComponentRegistry{10}; // ok, other instance gone
    EXPECT_FALSE(uut.IsTypeRegistered<S1>()); // statics should be cleared
}

TEST(ComponentRegistryTests, RegisterType_validCall_succeeds)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<int>(5);
    uut.RegisterType<std::string>(5);
    uut.RegisterType<S1>(10);
    uut.RegisterType<S2>(10);
    SUCCEED();
}

TEST(ComponentRegistryTests, RegisterType_typeAlreadyRegistered_throws)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10);
    EXPECT_THROW(uut.RegisterType<S1>(10), nc::NcError);
}

TEST(ComponentRegistryTests, RegisterType_idAlreadyUsed_throws)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10, nc::ComponentHandler<S1>{.id = 1000});
    EXPECT_THROW(uut.RegisterType<S2>(10, nc::ComponentHandler<S2>{.id = 1000}), nc::NcError);
}

TEST(ComponentRegistryTests, RegisterType_idCollidesWithInternallyAssignedId_throws)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10, nc::ComponentHandler<S1>{});
    EXPECT_THROW(uut.RegisterType<S2>(10, nc::ComponentHandler<S2>{.id = uut.GetPool<S1>().Id()}), nc::NcError);
}

TEST(ComponentRegistryTests, IsTypeRegistered_returnsCorrectState)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    EXPECT_FALSE(uut.IsTypeRegistered<S1>());
    uut.RegisterType<S1>(1);
    EXPECT_TRUE(uut.IsTypeRegistered<S1>());
}

TEST(ComponentRegistryTests, GetPool_registeredType_succeeds)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    const auto& asConst = uut;
    uut.RegisterType<S1>(10);
    EXPECT_NO_THROW(uut.GetPool<S1>());
    EXPECT_NO_THROW(asConst.GetPool<S1>());
}

TEST(ComponentRegistryTests, GetPool_unregisteredType_throws)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    EXPECT_THROW(uut.GetPool<S1>(), std::exception);
}

TEST(ComponentRegistryTests, GetPoolById_registeredType_succeeds)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    const auto& asConst = uut;
    uut.RegisterType<S1>(10, nc::ComponentHandler<S1>{.id = 200});
    EXPECT_NO_THROW(uut.GetPool(200));
    EXPECT_NO_THROW(asConst.GetPool(200));
}

TEST(ComponentRegistryTests, GetPoolById_unregisteredType_throws)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    EXPECT_THROW(uut.GetPool(200), nc::NcError);
}

TEST(ComponentRegistryTests, GetPool_entity_succeeds)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    const auto& asConst = uut;
    EXPECT_NO_THROW(uut.GetPool<nc::Entity>());
    EXPECT_NO_THROW(asConst.GetPool<nc::Entity>());
}

TEST(ComponentRegistryTests, GetBasePoools_returnsPools)
{
    auto uut = nc::ecs::ComponentRegistry{1};
    uut.RegisterType<S1>(1);
    uut.RegisterType<S2>(1);
    const auto pools = uut.GetComponentPools();
    EXPECT_EQ(2ull, pools.size());
}

TEST(ComponentRegistryTests, CommitPendingChanges_updatesState)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10);
    auto& entities = uut.GetPool<nc::Entity>();
    auto& components = uut.GetPool<S1>();
    const auto a = entities.Add(0, 0);
    const auto b = entities.Add(0, 0);
    components.Emplace(a);
    components.Emplace(b);
    entities.Remove(b);
    uut.CommitPendingChanges();
    EXPECT_TRUE(entities.Contains(a));
    EXPECT_TRUE(components.Contains(a));
    EXPECT_FALSE(entities.Contains(b));
    EXPECT_FALSE(components.Contains(b));
    const auto next = entities.Add(0, 0);
    EXPECT_EQ(next, b);
}

TEST(ComponentRegistryTests, ClearSceneData)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10);
    auto& entities = uut.GetPool<nc::Entity>();
    auto& components = uut.GetPool<S1>();
    const auto a = entities.Add(0, 0);
    const auto b = entities.Add(0, nc::Entity::Flags::Persistent);
    const auto c = entities.Add(0, 0);
    components.Emplace(a);
    components.Emplace(b);
    components.Emplace(c);
    uut.CommitPendingChanges();
    uut.ClearSceneData();
    EXPECT_EQ(1, entities.Size());
    EXPECT_EQ(1, components.Size());
    EXPECT_FALSE(entities.Contains(a));
    EXPECT_FALSE(components.Contains(a));
    EXPECT_TRUE(entities.Contains(b));
    EXPECT_TRUE(components.Contains(b));
    EXPECT_FALSE(entities.Contains(c));
    EXPECT_FALSE(components.Contains(c));
}

TEST(ComponentRegistryTests, Clear)
{
    auto uut = nc::ecs::ComponentRegistry{10};
    uut.RegisterType<S1>(10);
    auto& entities = uut.GetPool<nc::Entity>();
    auto& components = uut.GetPool<S1>();
    const auto a = entities.Add(0, 0);
    const auto b = entities.Add(0, nc::Entity::Flags::Persistent);
    const auto c = entities.Add(0, 0);
    components.Emplace(a);
    components.Emplace(b);
    components.Emplace(c);
    uut.CommitPendingChanges();
    uut.Clear();
    EXPECT_EQ(0, entities.Size());
    EXPECT_EQ(0, components.Size());
    EXPECT_FALSE(entities.Contains(a));
    EXPECT_FALSE(components.Contains(a));
    EXPECT_FALSE(entities.Contains(b));
    EXPECT_FALSE(components.Contains(b));
    EXPECT_FALSE(entities.Contains(c));
    EXPECT_FALSE(components.Contains(c));
}
