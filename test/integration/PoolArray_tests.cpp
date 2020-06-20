#include "gtest/gtest.h"
#include "engine/alloc/PoolArray.h"

using namespace nc::engine::alloc;

struct Mock
{
    int val = 0;
    nc::MemoryState memState = nc::MemoryState::Invalid;
    nc::MemoryState GetMemoryState() const { return memState; }
};


TEST(PoolArray, IsFull_PoolFull_ReturnsTrue)
{
    PoolArray<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, true);
    allocator.Free(pos);
}

TEST(PoolArray, IsFull_AfterFree_ReturnsFalse)
{
    PoolArray<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    allocator.Free(pos);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, false);
}

TEST(PoolArray, Alloc_PoolFull_Throws)
{
    PoolArray<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    EXPECT_THROW(allocator.Alloc(&mock), nc::NcException);
    allocator.Free(pos);
}

TEST(PoolArray, GetPtrTo_GoodArgs_GetsPtr)
{
    PoolArray<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    *mock = Mock{ 1, nc::MemoryState::Valid };
    auto actual = allocator.GetPtrTo(pos);
    EXPECT_EQ(actual->val, mock->val);
    EXPECT_EQ(actual->GetMemoryState(), nc::MemoryState::Valid);
    allocator.Free(pos);
}

TEST(PoolArray, GetPtrTo_BadArgs_Throws)
{
    PoolArray<Mock> allocator(1);
    EXPECT_THROW(allocator.GetPtrTo(1u), nc::NcException);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    //Init();
    return RUN_ALL_TESTS();
}