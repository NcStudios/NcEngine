#include "gtest/gtest.h"

#include "alloc/MemoryResource.h"

using namespace nc::alloc;

const size_t PoolDefaultSize = 16u;
const size_t LinearBufferDefaultSize = 16u;
const size_t DefaultAlignment = 16u;

struct Fake
{
    int val;
};

TEST(StaticPoolResource_tests, Allocate_MemoryAvailable_ReturnsNonNull)
{
    StaticPoolResource<Fake> pool(PoolDefaultSize);
    auto actual = pool.allocate(1u, DefaultAlignment);
    EXPECT_NE(actual, nullptr);
}

TEST(StaticPoolResource_tests, Allocate_ConsecutiveCallsOnEmptyPool_AllocateSequentially)
{
    StaticPoolResource<Fake> pool(PoolDefaultSize);
    auto first = pool.allocate(1u, DefaultAlignment);
    auto second = pool.allocate(1u, DefaultAlignment);
    auto third = pool.allocate(1u, DefaultAlignment);

    auto actual = second - first;
    EXPECT_EQ(actual, 1u);
    actual = third - second;
    EXPECT_EQ(actual, 1u);
}

TEST(StaticPoolResource_tests, Allocate_CallAfterDeallocate_AllocatesFromFreeList)
{
    StaticPoolResource<Fake> pool(PoolDefaultSize);
    auto expected = pool.allocate(1u, DefaultAlignment);
    pool.deallocate(expected, 1u);
    auto actual = pool.allocate(1u, DefaultAlignment);
    EXPECT_EQ(actual, expected);
}

TEST(StaticPoolResource_tests, Allocate_MemoryNotAvailable_Throws)
{
    StaticPoolResource<Fake> pool(sizeof(Fake));
    pool.allocate(1u, DefaultAlignment);
    EXPECT_THROW(pool.allocate(1u, DefaultAlignment), MemoryResourceBadAlloc);
}

TEST(StaticPoolResource_tests, Allocate_CallAfterFreeAll_AllocatesFromBegining)
{
    StaticPoolResource<Fake> pool(PoolDefaultSize);
    auto expected = pool.allocate(1u, DefaultAlignment);
    pool.allocate(1u, DefaultAlignment);
    auto toRemove = pool.allocate(1u, DefaultAlignment);
    pool.deallocate(toRemove, 1u); // put something in free list
    pool.free_all();
    auto actual = pool.allocate(1u, DefaultAlignment);
    EXPECT_EQ(actual, expected);
}

TEST(LinearBufferResource_tests, Allocate_MemoryAvailable_ReturnsNonNull)
{
    LinearBufferResource buffer(LinearBufferDefaultSize);
    auto actual = buffer.allocate(sizeof(Fake), alignof(Fake));
    EXPECT_NE(actual, nullptr);
}

TEST(LinearBufferResource_tests, Allocate_BufferFull_Throws)
{
    LinearBufferResource buffer(LinearBufferDefaultSize);
    buffer.allocate(LinearBufferDefaultSize, DefaultAlignment);
    EXPECT_THROW(buffer.allocate(sizeof(Fake), alignof(Fake)), MemoryResourceBadAlloc);
}

TEST(LinearBufferResource_tests, Allocate_ConsecutiveCalls_AllocateSequentially)
{
    LinearBufferResource buffer(LinearBufferDefaultSize);
    auto first = buffer.allocate(sizeof(Fake), alignof(Fake));
    auto second = buffer.allocate(sizeof(Fake), alignof(Fake));
    EXPECT_GT(second, first);
}

TEST(LinearBufferResource_tests, Allocate_CallAfterClear_AllocatesFromBeginning)
{
    LinearBufferResource buffer(LinearBufferDefaultSize);
    auto expected = buffer.allocate(sizeof(Fake), alignof(Fake));
    buffer.free_all();
    auto actual = buffer.allocate(sizeof(Fake), alignof(Fake));
    EXPECT_EQ(actual, expected);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}