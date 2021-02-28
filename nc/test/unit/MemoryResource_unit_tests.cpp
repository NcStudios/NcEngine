#include "gtest/gtest.h"

#include "alloc/MemoryResource.h"

using namespace nc::alloc;

const size_t DefaultSize = 10u;

struct Fake
{
    int val;
};

TEST(StaticPool_unit_tests, Allocate_MemoryAvailable_ReturnsNonNull)
{
    StaticPool<Fake> pool(DefaultSize);
    auto actual = pool.allocate(1u);
    EXPECT_NE(actual, nullptr);
}

TEST(StaticPool_unit_tests, Allocate_ConsecutiveCallsOnEmptyPool_AllocateSequentially)
{
    StaticPool<Fake> pool(DefaultSize);
    auto first = pool.allocate(1u);
    auto second = pool.allocate(1u);
    auto third = pool.allocate(1u);

    auto actual = second - first;
    EXPECT_EQ(actual, 1u);
    actual = third - second;
    EXPECT_EQ(actual, 1u);
}

TEST(StaticPool_unit_tests, Allocate_CallAfterDeallocate_AllocatesFromFreeList)
{
    StaticPool<Fake> pool(DefaultSize);
    auto expected = pool.allocate(1u);
    pool.deallocate(expected, 1u);
    auto actual = pool.allocate(1u);
    EXPECT_EQ(actual, expected);
}

TEST(StaticPool_unit_tests, Allocate_MemoryNotAvailable_Throws)
{
    StaticPool<Fake> pool(1u);
    pool.allocate(1u);
    EXPECT_THROW(pool.allocate(1u), MemoryResourceBadAlloc);
}

TEST(StaticPool_unit_tests, Allocate_CallAfterFreeAll_AllocatesFromBegining)
{
    StaticPool<Fake> pool(DefaultSize);
    auto expected = pool.allocate(1u);
    pool.allocate(1u);
    auto toRemove = pool.allocate(1u);
    pool.deallocate(toRemove, 1u); // put something in free list
    pool.free_all();
    auto actual = pool.allocate(1u);
    EXPECT_EQ(actual, expected);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}