#include "gtest/gtest.h"
#include "physics/collision/narrow_phase/PairCache.h"

using namespace nc;

constexpr Entity entity1 = Entity{1u, 0u, Entity::Flags::None};
constexpr Entity entity2 = Entity{2u, 0u, Entity::Flags::None};
constexpr Entity entity3 = Entity{3u, 0u, Entity::Flags::None};
constexpr Entity entity4 = Entity{4u, 0u, Entity::Flags::None};

struct Entry
{
    Entity a;
    Entity b;
    int value;
};

struct TestCache : public PairCache<TestCache, Entry, Entity, 128u>
{
    size_t pairEqualCalls = 0u;
    size_t addToExistingCalls = 0u;
    size_t constructNewCalls = 0u;
    size_t hashCalls = 0u;

    bool PairEqual(Entity a, Entity b, const Entry& entry)
    {
        ++pairEqualCalls;
        return (a == entry.a && b == entry.b) ||
               (a == entry.b && b == entry.a);
    }

    void AddToExisting(Entry* existing, int v)
    {
        ++addToExistingCalls;
        existing->value = v;
    }

    auto ConstructNew(Entity a, Entity b, int v) -> Entry
    {
        ++constructNewCalls;
        return Entry{a, b, v};
    }

    auto Hash(const Entry& entry) -> uint32_t
    {
        ++hashCalls;
        return PairCache<TestCache, Entry, Entity, 128u>::Hash(entry.a, entry.b);
    }
};

class PairCache_unit_tests : public ::testing::Test
{
    public:
        PairCache_unit_tests() : cache{} {}
        ~PairCache_unit_tests() {}

        TestCache cache;
};

TEST_F(PairCache_unit_tests, Add_NewEntry_CallsConstructNew)
{
    ASSERT_EQ(cache.constructNewCalls, 0);
    cache.Add(entity1, entity2, 3);
    EXPECT_EQ(cache.constructNewCalls, 1);
}

TEST_F(PairCache_unit_tests, Add_ExistingEntry_CallsAddToExisting)
{
    ASSERT_EQ(cache.addToExistingCalls, 0);
    cache.Add(entity1, entity2, 3);
    cache.Add(entity1, entity2, 3);
    EXPECT_EQ(cache.addToExistingCalls, 1);
}

TEST_F(PairCache_unit_tests, Remove_ExistingEntry_RemovesEntry)
{
    cache.Add(entity4, entity3, 3);
    ASSERT_EQ(cache.Data().size(), 1);
    cache.Remove(entity4, entity3);
    auto data = cache.Data();
    EXPECT_EQ(cache.Data().size(), 0);
}

TEST_F(PairCache_unit_tests, Remove_Empty_EarlyExit)
{
    ASSERT_EQ(cache.hashCalls, 0);
    ASSERT_EQ(cache.Data().size(), 0);
    cache.Remove(entity1, entity2);
    EXPECT_EQ(cache.hashCalls, 0);
    ASSERT_EQ(cache.Data().size(), 0);
}

TEST_F(PairCache_unit_tests, Find_ExistingEntry_ReturnsPointer)
{
    cache.Add(entity1, entity2, 3);
    auto hash = cache.Hash(Entry{entity1, entity2, 3});
    auto* actual = cache.Find(entity1, entity2, hash);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(actual->a, entity1);
    EXPECT_EQ(actual->b, entity2);
    EXPECT_EQ(actual->value, 3);
}

TEST_F(PairCache_unit_tests, Find_ReversedPair_ReturnsPointer)
{
    cache.Add(entity1, entity2, 3);
    auto hash = cache.Hash(Entry{entity2, entity1, 3});
    auto* actual = cache.Find(entity2, entity1, hash);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(actual->a, entity1);
    EXPECT_EQ(actual->b, entity2);
    EXPECT_EQ(actual->value, 3);
}

TEST_F(PairCache_unit_tests, Find_MultipleExistingEntries_ReturnsCorrectPointer)
{
    cache.Add(entity1, entity2, 1);
    cache.Add(entity2, entity3, 2);
    cache.Add(entity3, entity4, 3);
    cache.Add(entity4, entity1, 4);
    auto hash = cache.Hash(Entry{entity3, entity4, 3});
    auto* actual = cache.Find(entity3, entity4, hash);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(actual->a, entity3);
    EXPECT_EQ(actual->b, entity4);
    EXPECT_EQ(actual->value, 3);
}

TEST_F(PairCache_unit_tests, Find_BadEntry_ReturnsNull)
{
    auto hash = cache.Hash(Entry{entity1, entity2, 3});
    auto* actual = cache.Find(entity1, entity2, hash);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(PairCache_unit_tests, Find_RemovedEntry_ReturnsNull)
{
    cache.Add(entity1, entity2, 1);
    cache.Add(entity2, entity3, 2);
    cache.Add(entity3, entity4, 3);
    cache.Add(entity4, entity1, 4);
    cache.Remove(entity2, entity3);
    auto hash = cache.Hash(Entry{entity2, entity3, 2});
    auto* actual = cache.Find(entity2, entity3, hash);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(PairCache_unit_tests, Find_AfterClear_ReturnsNull)
{
    cache.Add(entity1, entity2, 1);
    cache.Add(entity2, entity3, 2);
    cache.Add(entity3, entity4, 3);
    cache.Add(entity4, entity1, 4);
    cache.Clear();
    auto hash = cache.Hash(Entry{entity2, entity3, 2});
    auto* actual = cache.Find(entity2, entity3, hash);
    EXPECT_EQ(actual, nullptr);
}

TEST_F(PairCache_unit_tests, Hash_IdTypeEqualsEntity_DoesNotRequireDerivedHash)
{
    ASSERT_EQ(cache.hashCalls, 0);
    static_cast<PairCache<TestCache, Entry, Entity, 128u>*>(&cache)->Hash(entity1, entity2);
    EXPECT_EQ(cache.hashCalls, 0);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}