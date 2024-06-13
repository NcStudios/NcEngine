#include "gtest/gtest.h"
#include "ncengine/ecs/EntityPool.h"

TEST(EntityPoolTests, Add_returnsNewEntities)
{
    auto uut = nc::ecs::EntityPool{5ull};
    const auto a = uut.Add(0, 0, 0);
    const auto b = uut.Add(0, 0, 0);
    const auto c = uut.Add(1, 2, 0);
    EXPECT_EQ(3, uut.size());
    EXPECT_TRUE(a.Valid());
    EXPECT_TRUE(b.Valid());
    EXPECT_TRUE(c.Valid());
    EXPECT_NE(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(b, c);
    EXPECT_EQ(1, c.Layer());
    EXPECT_EQ(2, c.Flags());
}

TEST(EntityPoolTests, Remove_removesFromPool)
{
    auto uut = nc::ecs::EntityPool{5ull};
    const auto a = uut.Add(0, 0, 0);
    EXPECT_TRUE(uut.Contains(a));
    uut.Remove(a);
    EXPECT_FALSE(uut.Contains(a));
}

TEST(EntityPoolTests, RecycleDeadEntities_reclaimsHandles)
{
    auto uut = nc::ecs::EntityPool{5ull};
    const auto a = uut.Add(0, 0, 0);
    const auto b = uut.Add(0, 0, 0);
    uut.Remove(a);
    uut.Remove(b);

    const auto notRecycled = uut.Add(0, 0, 0); // should not reuse handles yet
    EXPECT_NE(notRecycled, a);
    EXPECT_NE(notRecycled, b);

    const auto dead = uut.RecycleDeadEntities();
    ASSERT_EQ(2, dead.size());
    EXPECT_EQ(a, dead.at(0));
    EXPECT_EQ(b, dead.at(1));

    const auto recycled = uut.Add(0, 0, 0); // should reuse one of the removed handles
    EXPECT_TRUE(recycled == a || recycled == b);
}

TEST(EntityPoolTests, ClearNonPersistent_leavesPersistentEntities)
{
    auto uut = nc::ecs::EntityPool{5ull};
    const auto a = uut.Add(0, 0, 0);
    const auto b = uut.Add(0, nc::Entity::Flags::Persistent, 0);
    const auto c = uut.Add(0, 0, 0);
    const auto d = uut.Add(0, nc::Entity::Flags::Persistent, 0);
    uut.ClearNonPersistent();
    EXPECT_EQ(2, uut.Size());
    EXPECT_FALSE(uut.Contains(a));
    EXPECT_TRUE(uut.Contains(b));
    EXPECT_FALSE(uut.Contains(c));
    EXPECT_TRUE(uut.Contains(d));

    // Check that only the non-persistent handles were recycled. We don't what order they'll
    // come back in, so collect them + sort before comparing.
    auto next = std::vector<nc::Entity>{};
    std::ranges::generate_n(std::back_inserter(next), 2, [&uut]() { return uut.Add(0, 0, 0); });
    std::ranges::sort(next, [](auto l, auto r) { return l.Index() < r.Index(); });
    EXPECT_EQ(next.at(0), a);
    EXPECT_EQ(next.at(1), c);
}

TEST(EntityPoolTests, Clear_clearsAll)
{
    auto uut = nc::ecs::EntityPool{5ull};
    const auto a = uut.Add(0, 0, 0);
    const auto b = uut.Add(0, nc::Entity::Flags::Persistent, 0);
    const auto c = uut.Add(0, 0, 0);
    const auto d = uut.Add(0, nc::Entity::Flags::Persistent, 0);
    uut.Clear();
    EXPECT_EQ(0, uut.Size());
    EXPECT_FALSE(uut.Contains(a));
    EXPECT_FALSE(uut.Contains(b));
    EXPECT_FALSE(uut.Contains(c));
    EXPECT_FALSE(uut.Contains(d));

    // Check that all the handles were recycled. Same ordering issue as the previous test.
    auto next = std::vector<nc::Entity>{};
    std::ranges::generate_n(std::back_inserter(next), 4, [&uut]() { return uut.Add(0, 0, 0); });
    std::ranges::sort(next, [](auto l, auto r) { return l.Index() < r.Index(); });
    EXPECT_EQ(next.at(0), a);
    EXPECT_EQ(next.at(1), b);
    EXPECT_EQ(next.at(2), c);
    EXPECT_EQ(next.at(3), d);
}

TEST(EntityPoolTests, StlViewInterface_hasExpectedFunctions)
{
    auto uut = nc::ecs::EntityPool{5ull};
    auto entities = std::vector<nc::Entity>{ uut.Add(0, 0, 0) };

    ASSERT_EQ(entities.size(), uut.size());
    EXPECT_FALSE(uut.empty());
    EXPECT_EQ(*entities.data(), *uut.data());
    EXPECT_EQ(entities[0], uut[0]);
    EXPECT_EQ(entities.at(0), uut.at(0));
    EXPECT_TRUE(std::ranges::equal(entities, uut));
}
