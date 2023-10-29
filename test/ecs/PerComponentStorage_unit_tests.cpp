#include "ncengine/ecs/detail/PerComponentStorage.h"
#include "gtest/gtest.h"

// note: Most PerComponentStorage methods end up being indirectly tested by Registry_unit_tests,
//       so we're just covering some of the virtual/handler methods.

struct Foo
{
    int v;
};

auto g_userData = 1;
const auto g_fooHandler = nc::ComponentHandler<Foo>
{
    .name = "Foo",
    .userData = &g_userData,
    .factory = [](nc::Entity, void*){return Foo{1};},
    .drawUI = [](Foo&) {}
};

TEST(PerComponentStorageTests, Constructor_defaultHandler_hasDefaultValues)
{
    auto handler = nc::ComponentHandler<Foo>{};
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, handler};
    EXPECT_EQ(handler.name, uut.GetComponentName());
    EXPECT_FALSE(uut.HasFactory());
    EXPECT_FALSE(uut.HasUserData());
    EXPECT_FALSE(uut.HasDrawUI());
}

TEST(PerComponentStorageTests, Constructor_customHandler_hasCustomValues)
{
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, g_fooHandler};
    EXPECT_EQ(g_fooHandler.name, uut.GetComponentName());
    EXPECT_TRUE(uut.HasFactory());
    EXPECT_TRUE(uut.HasUserData());
    EXPECT_TRUE(uut.HasDrawUI());
}

TEST(PerComponentStorageTests, GetAsAnyComponent_validEntity_returnsValidObject)
{
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, g_fooHandler};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Add(entity, 1);
    const auto actual = uut.GetAsAnyComponent(entity);
    EXPECT_TRUE(actual);
}

TEST(PerComponentStorageTests, GetAsAnyComponent_badEntity_returnsNull)
{
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, g_fooHandler};
    const auto entity = nc::Entity{1, 0, 0};
    const auto actual = uut.GetAsAnyComponent(entity);
    EXPECT_FALSE(actual);
}

TEST(PerComponentStorageTests, AddDefault_hasUserData_passesData)
{
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, g_fooHandler};
    ASSERT_TRUE(uut.HasFactory());
    const auto entity = nc::Entity{1, 0, 0};
    auto actual = uut.AddDefault(entity);
    EXPECT_TRUE(actual);
    uut.CommitStagedComponents({});
    EXPECT_EQ(1, uut.Size());
}

TEST(PerComponentStorageTests, AddDefault_noFactory_returnsNull)
{
    auto uut = nc::ecs::detail::PerComponentStorage<Foo>{10, nc::ComponentHandler<Foo>{}};
    const auto entity = nc::Entity{1, 0, 0};
    auto actual = uut.AddDefault(entity);
    EXPECT_FALSE(actual);
    uut.CommitStagedComponents({});
    EXPECT_EQ(0, uut.Size());
}
