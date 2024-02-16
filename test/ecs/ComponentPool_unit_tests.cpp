#include "gtest/gtest.h"
#include "ncengine/ecs/ComponentPool.h"

#include <algorithm>
#include <ranges>

struct S1
{
    int value = 0;
};

TEST(ComponentPoolTests, Emplace_validCall_attachesComponent)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    constexpr auto expectedValue = 42;
    auto& actual = uut.Emplace(nc::Entity{1, 0, 0}, expectedValue);
    EXPECT_EQ(actual.value, expectedValue);
}

TEST(ComponentPoolTests, Emplace_alreadyHasComponent_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity, 42);
    EXPECT_THROW(uut.Emplace(entity), nc::NcError);
}

TEST(ComponentPoolTests, Emplace_badEntity_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_THROW(uut.Emplace(nc::Entity::Null(), 42), nc::NcError); // null entity
    EXPECT_THROW(uut.Emplace(nc::Entity{11, 0, 0}), nc::NcError); // index exceeds capacity
}

TEST(ComponentPoolTests, Insert_validCall_attachesComponent)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    constexpr auto expected = S1{42};
    auto& actual = uut.Insert(nc::Entity{1, 0, 0}, expected);
    EXPECT_EQ(actual.value, expected.value);
}

TEST(ComponentPoolTests, Insert_alreadyHasComponent_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    const auto instance = S1{};
    uut.Insert(entity, instance);
    EXPECT_THROW(uut.Insert(entity, instance), nc::NcError);
}

TEST(ComponentPoolTests, Insert_badEntity_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto instance = S1{};
    EXPECT_THROW(uut.Insert(nc::Entity::Null(), instance), nc::NcError); // null entity
    EXPECT_THROW(uut.Insert(nc::Entity{11, 0, 0}, instance), nc::NcError); // index exceeds capacity
}

TEST(ComponentPoolTests, Remove_stagedComponent_returnsTrue)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    EXPECT_TRUE(uut.Remove(entity));
    EXPECT_FALSE(uut.Contains(entity));
}

TEST(ComponentPoolTests, Remove_committedComponent_returnsTrue)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    uut.CommitStagedComponents({});
    EXPECT_TRUE(uut.Remove(entity));
    EXPECT_FALSE(uut.Contains(entity));
}

TEST(ComponentPoolTests, Remove_entityDoesNotExist_returnsFalse)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_FALSE(uut.Remove(nc::Entity{1, 0, 0}));
}

TEST(ComponentPoolTests, Remove_badEntity_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_THROW(uut.Remove(nc::Entity::Null()), nc::NcError);
}

TEST(ComponentPoolTests, Get_stagedComponent_succeeds)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    EXPECT_NO_THROW(uut.Get(entity));
}

TEST(ComponentPoolTests, Get_committedComponent_succeeds)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    uut.CommitStagedComponents({});
    EXPECT_NO_THROW(uut.Get(entity));
}

TEST(ComponentPoolTests, Get_componentDoesNotExist_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_THROW(uut.Get(nc::Entity{1, 0, 0}), nc::NcError);
}

TEST(ComponentPoolTests, Get_badEntity_throws)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_THROW(uut.Get(nc::Entity::Null()), nc::NcError);
}

TEST(ComponentPoolTests, GetParent_stagedComponent_returnsEntity)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto expected = nc::Entity{1, 0, 0};
    const auto& component = uut.Emplace(expected);
    const auto actual = uut.GetParent(&component);
    EXPECT_EQ(expected, actual);
}

TEST(ComponentPoolTests, GetParent_committedComponent_returnsEntity)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto expected = nc::Entity{1, 0, 0};
    uut.Emplace(expected);
    uut.CommitStagedComponents({});
    const auto& component = uut.Get(expected);
    const auto actual = uut.GetParent(&component);
    EXPECT_EQ(expected, actual);
}

TEST(ComponentPoolTests, GetParent_componentDeleted_returnsNullEntity)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    uut.CommitStagedComponents({});
    const auto& soonToExpire = uut.Get(entity);
    uut.Remove(entity);
    uut.CommitStagedComponents({});

    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(&soonToExpire));
}

TEST(ComponentPoolTests, GetParent_nullComponent_returnsNullEntity)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(nullptr));
}

TEST(ComponentPoolTests, GetAsAnyComponent_exists_returnsValidObject)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    uut.Emplace(entity);
    const auto actual = uut.GetAsAnyComponent(entity);
    EXPECT_TRUE(actual);
}

TEST(ComponentPoolTests, GetAsAnyComponent_doesNotExist_returnsNullObject)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{1, 0, 0};
    const auto actual = uut.GetAsAnyComponent(entity);
    EXPECT_FALSE(actual);
}

TEST(ComponentPoolTests, Size_allVariations_returnsExpectedValues)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    EXPECT_EQ(0, uut.Size());
    EXPECT_EQ(0, uut.size());
    EXPECT_EQ(0, uut.StagedSize());
    EXPECT_EQ(0, uut.TotalSize());

    uut.Emplace(nc::Entity{0, 0, 0});
    EXPECT_EQ(0, uut.Size());
    EXPECT_EQ(0, uut.size());
    EXPECT_EQ(1, uut.StagedSize());
    EXPECT_EQ(1, uut.TotalSize());
    uut.CommitStagedComponents({});

    EXPECT_EQ(1, uut.Size());
    EXPECT_EQ(1, uut.size());
    EXPECT_EQ(0, uut.StagedSize());
    EXPECT_EQ(1, uut.TotalSize());

    uut.Emplace(nc::Entity{1, 0, 0});
    EXPECT_EQ(1, uut.Size());
    EXPECT_EQ(1, uut.size());
    EXPECT_EQ(1, uut.StagedSize());
    EXPECT_EQ(2, uut.TotalSize());
}

TEST(ComponentPoolTests, ClearNonPersistent_leavesPersistentData)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto a = nc::Entity{0, 0, 0};
    const auto b = nc::Entity{1, 0, nc::Entity::Flags::Persistent};
    const auto c = nc::Entity{2, 0, 0};
    uut.Emplace(a);
    uut.Emplace(b);
    uut.Emplace(c);
    uut.CommitStagedComponents({});
    uut.ClearNonPersistent();
    EXPECT_EQ(1, uut.Size());
    EXPECT_FALSE(uut.Contains(a));
    EXPECT_TRUE(uut.Contains(b));
    EXPECT_FALSE(uut.Contains(c));
}

TEST(ComponentPoolTests, Clear_clearsAllData)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto a = nc::Entity{0, 0, 0};
    const auto b = nc::Entity{1, 0, nc::Entity::Flags::Persistent};
    const auto c = nc::Entity{2, 0, 0};
    uut.Emplace(a);
    uut.Emplace(b);
    uut.Emplace(c);
    uut.CommitStagedComponents({});
    uut.Clear();
    EXPECT_EQ(0, uut.Size());
    EXPECT_FALSE(uut.Contains(a));
    EXPECT_FALSE(uut.Contains(b));
    EXPECT_FALSE(uut.Contains(c));
}

TEST(ComponentPoolTests, Reserve_subsequentAdds_preserveReferenceValidity)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entityA = nc::Entity{0, 0, 0};
    const auto entityB = nc::Entity{1, 0, nc::Entity::Flags::Persistent};
    const auto entityC = nc::Entity{2, 0, 0};
    const auto entityD = nc::Entity{3, 0, 0};
    const auto entityE = nc::Entity{4, 0, 0};

    uut.Reserve(5);
    uut.Emplace(entityA);
    uut.CommitStagedComponents({});
    const auto& expectedA = uut.Get(entityA);

    const auto& expectedB = uut.Emplace(nc::Entity{1, 0, 0});
    const auto& expectedC = uut.Emplace(nc::Entity{2, 0, 0});
    const auto& expectedD = uut.Emplace(nc::Entity{3, 0, 0});
    const auto& expectedE = uut.Emplace(nc::Entity{4, 0, 0});
    const auto& actualA = uut.Get(entityA);
    const auto& actualB = uut.Get(entityB);
    const auto& actualC = uut.Get(entityC);
    const auto& actualD = uut.Get(entityD);
    const auto& actualE = uut.Get(entityE);

    EXPECT_EQ(&expectedA, &actualA);
    EXPECT_EQ(&expectedB, &actualB);
    EXPECT_EQ(&expectedC, &actualC);
    EXPECT_EQ(&expectedD, &actualD);
    EXPECT_EQ(&expectedE, &actualE);
}

TEST(ComponentPoolTests, StlViewInterface_hasExpectedFunctions)
{
    auto uut = nc::ecs::ComponentPool<S1>{10u, nc::ComponentHandler<S1>{}};
    const auto entity = nc::Entity{0, 0, 0};
    uut.Emplace(entity);
    uut.CommitStagedComponents({});
    auto components = std::vector<S1*>{};
    components.push_back(&uut.Get(entity));
    components.push_back(&uut.Emplace(nc::Entity{1, 0, 0}));

    ASSERT_EQ(1, uut.size()); // second item still staged so size only 1
    EXPECT_FALSE(uut.empty());
    EXPECT_EQ(*components.data(), uut.data());
    EXPECT_EQ(components[0], &uut[0]);

    const auto asRange = std::ranges::subrange(uut);
    ASSERT_EQ(1, asRange.size());
    EXPECT_EQ(components.at(0), &asRange[0]);
}
