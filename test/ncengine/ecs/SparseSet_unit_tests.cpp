#include "gtest/gtest.h"
#include "ncengine/ecs/detail/SparseSet.h"

using SparseSet_t = nc::ecs::detail::SparseSet<int>;
constexpr auto g_entity = nc::Entity{1, 0, 0};

TEST(SparseSetTests, Insert_noComponentOnEntity_addsComponent)
{
    auto uut = SparseSet_t{10};
    uut.Insert(g_entity, 42);
    const auto packed = uut.GetPackedArray();
    ASSERT_EQ(1, packed.size());
    EXPECT_EQ(42, packed[0]);
}

TEST(SparseSetTests, Insert_componentAlreadyOnEntity_throws)
{
    auto uut = SparseSet_t{10};
    uut.Insert(g_entity, 42);
    EXPECT_THROW(uut.Insert(g_entity, 43), std::exception);
}

TEST(SparseSetTests, Remove_componentExists_returnsTrue)
{
    auto uut = SparseSet_t{10};
    uut.Insert(g_entity, 1);
    EXPECT_TRUE(uut.Remove(g_entity));
}

TEST(SparseSetTests, Remove_componentDoesNotExist_returnsFalse)
{
    auto uut = SparseSet_t{10};
    EXPECT_FALSE(uut.Remove(g_entity));
}

TEST(SparseSetTests, Get_exists_returnsComponent)
{
    auto uut = SparseSet_t{10};
    uut.Insert(g_entity, 42);
    EXPECT_EQ(42, uut.Get(g_entity));
}

TEST(SparseSetTests, Get_doesNotExist_throws)
{
    auto uut = SparseSet_t{10};
    EXPECT_THROW(uut.Get(g_entity), std::exception);
}

TEST(SparseSetTests, GetParent_exists_returnsParentEntity)
{
    auto uut = SparseSet_t{10};
    auto& component = uut.Insert(g_entity, 42);
    EXPECT_EQ(g_entity, uut.GetParent(&component));
}

TEST(SparseSetTests, GetParent_doesNotExist_returnsNullEntity)
{
    auto uut = SparseSet_t{10};
    auto& component = uut.Insert(g_entity, 42);
    uut.Remove(g_entity);
    EXPECT_EQ(nc::Entity::Null(), uut.GetParent(&component)); // tolerates dangling pointers
}

TEST(SparseSetTests, GetIndex_exists_returnsIndex)
{
    const auto first = nc::Entity{9, 0, 0};
    const auto second = nc::Entity{7, 0, 0};
    const auto third = nc::Entity{2, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(first, 0);
    uut.Insert(second, 0);
    uut.Insert(third, 0);
    EXPECT_EQ(0u, uut.GetIndex(first));
    EXPECT_EQ(1u, uut.GetIndex(second));
    EXPECT_EQ(2u, uut.GetIndex(third));
}

TEST(SparseSetTests, GetIndex_doesNotExist_returnsNullIndex)
{
    auto uut = SparseSet_t{10};
    EXPECT_EQ(nc::Entity::NullIndex, uut.GetIndex(g_entity));
}

TEST(SparseSetTests, GetIndex_outOfBounds_throws)
{
    auto uut = SparseSet_t{10};
    const auto outOfBounds = nc::Entity{10, 0, 0}; // indices are from [0, 10)
    EXPECT_THROW(uut.GetIndex(outOfBounds), std::exception);
}

TEST(SparseSetTests, GetEntity_exists_returnsEntity)
{
    const auto first = nc::Entity{9, 0, 0};
    const auto second = nc::Entity{7, 0, 0};
    const auto third = nc::Entity{2, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(first, 0);
    uut.Insert(second, 0);
    uut.Insert(third, 0);
    EXPECT_EQ(first, uut.GetEntity(first.Index()));
    EXPECT_EQ(second, uut.GetEntity(second.Index()));
    EXPECT_EQ(third, uut.GetEntity(third.Index()));
}

TEST(SparseSetTests, GetEntity_doesNotExist_throws)
{
    auto uut = SparseSet_t{10};
    EXPECT_THROW(uut.GetEntity(g_entity.Index()), std::exception);
}

TEST(SparseSetTests, GetEntity_outOfBounds_throws)
{
    auto uut = SparseSet_t{10};
    EXPECT_THROW(uut.GetEntity(10u), std::exception);
}

TEST(SparseSetTests, Contains_exists_returnsTrue)
{
    auto uut = SparseSet_t{10};
    uut.Insert(g_entity, 42);
    EXPECT_TRUE(uut.Contains(g_entity));
}

TEST(SparseSetTests, Contains_doesNotExist_returnsFalse)
{
    auto uut = SparseSet_t{10};
    EXPECT_FALSE(uut.Contains(g_entity));
}

TEST(SparseSetTests, Swap_validEntities_swapsPositions)
{
    const auto first = nc::Entity{9, 0, 0};
    const auto second = nc::Entity{7, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(first, 0);
    uut.Insert(second, 0);
    ASSERT_EQ(0, uut.GetIndex(first));
    ASSERT_EQ(1, uut.GetIndex(second));
    uut.Swap(first, second);
    EXPECT_EQ(1, uut.GetIndex(first));
    EXPECT_EQ(0, uut.GetIndex(second));
}

TEST(SparseSetTests, Swap_sameEntity_succeeds)
{
    const auto entity = nc::Entity{1, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(entity, 0);
    uut.Swap(entity, entity);
    EXPECT_EQ(0, uut.GetIndex(entity));
}

TEST(SparseSetTests, Swap_invalidEntity_throws)
{
    const auto good = nc::Entity{1, 0, 0};
    const auto bad = nc::Entity{2, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(good, 0);
    EXPECT_THROW(uut.Swap(good, bad), std::exception);
}

TEST(SparseSetTests, ClearNonPersistent_leavesPersistentData)
{
    const auto persistent = nc::Entity{4, 0, nc::Entity::Flags::Persistent};
    const auto throwAway1 = nc::Entity{0, 0, 0};
    const auto throwAway2 = nc::Entity{8, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(throwAway1, 0);
    uut.Insert(persistent, 0);
    uut.Insert(throwAway2, 0);
    uut.ClearNonPersistent();
    EXPECT_TRUE(uut.Contains(persistent));
    EXPECT_FALSE(uut.Contains(throwAway1));
    EXPECT_FALSE(uut.Contains(throwAway2));
    EXPECT_EQ(1, uut.GetPackedArray().size());
    EXPECT_EQ(1, uut.GetEntities().size());
}

TEST(SparseSetTests, Clear_clearsEverything)
{
    const auto persistent = nc::Entity{1, 0, nc::Entity::Flags::Persistent};
    const auto normal = nc::Entity{2, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(normal, 0);
    uut.Insert(persistent, 0);
    uut.Clear();
    EXPECT_FALSE(uut.Contains(persistent));
    EXPECT_FALSE(uut.Contains(normal));
    EXPECT_EQ(0, uut.GetPackedArray().size());
    EXPECT_EQ(0, uut.GetEntities().size());
}

TEST(SparseSetTests, Sort_ordersData)
{
    const auto first = nc::Entity{9, 0, 0};
    const auto second = nc::Entity{7, 0, 0};
    const auto third = nc::Entity{2, 0, 0};
    auto uut = SparseSet_t{10};
    uut.Insert(first, 300);
    uut.Insert(second, 200);
    uut.Insert(third, 100);
    uut.Sort(std::less<int>{});
    const auto& entities = uut.GetEntities();
    const auto& packed = uut.GetPackedArray();
    EXPECT_EQ(third, entities[0]);
    EXPECT_EQ(100, packed[0]);
    EXPECT_EQ(second, entities[1]);
    EXPECT_EQ(200, packed[1]);
    EXPECT_EQ(first, entities[2]);
    EXPECT_EQ(300, packed[2]);
}
