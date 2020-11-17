#include "gtest/gtest.h"
#include "engine/alloc/Pool.h"

using namespace nc::engine::alloc;

struct Mock
{
    int val = 0;
    nc::MemoryState memState = nc::MemoryState::Invalid;
    nc::MemoryState GetMemoryState() const { return memState; }
};

TEST(Pool_unit_tests, IsFull_PoolFull_ReturnsTrue)
{
    Pool<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, true);
    allocator.Free(pos);
}

TEST(Pool_unit_tests, IsFull_AfterFree_ReturnsFalse)
{
    Pool<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    allocator.Free(pos);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, false);
}

TEST(Pool_unit_tests, Alloc_PoolFull_Throws)
{
    Pool<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    EXPECT_THROW(allocator.Alloc(&mock), std::runtime_error);
    allocator.Free(pos);
}

TEST(Pool_unit_tests, GetPtrTo_GoodArgs_GetsPtr)
{
    Pool<Mock> allocator(1);
    Mock * mock = nullptr;
    auto pos = allocator.Alloc(&mock);
    *mock = Mock{1, nc::MemoryState::Valid};
    auto actual = allocator.GetPtrTo(pos);
    EXPECT_EQ(actual->val, mock->val);
    EXPECT_EQ(actual->GetMemoryState(), mock->GetMemoryState());
    allocator.Free(pos);
}

TEST(Pool_unit_tests, GetPtrTo_BadArgs_Throws)
{
    Pool<Mock> allocator(1);
    EXPECT_THROW(allocator.GetPtrTo(1u), std::runtime_error);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}