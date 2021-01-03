#include "gtest/gtest.h"
#include "engine/alloc/Pool.h"

using namespace nc::engine::alloc;

struct Mock
{
    Mock(int in) : val{in} {}
    int val = 0;
};

auto TestArg = 5;

TEST(Pool_unit_tests, IsFull_PoolFull_ReturnsTrue)
{
    Pool<Mock> allocator(1);
    auto pos = allocator.Alloc(TestArg);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, true);
    allocator.Free(pos);
}

TEST(Pool_unit_tests, IsFull_AfterFree_ReturnsFalse)
{
    Pool<Mock> allocator(1);
    auto pos = allocator.Alloc(TestArg);
    allocator.Free(pos);
    auto actual = allocator.IsFull();
    EXPECT_EQ(actual, false);
}

TEST(Pool_unit_tests, Alloc_PoolFull_Throws)
{
    Pool<Mock> allocator(1);
    auto pos = allocator.Alloc(TestArg);
    EXPECT_THROW(allocator.Alloc(TestArg), std::runtime_error);
    allocator.Free(pos);
}

TEST(Pool_unit_tests, GetPtrTo_GoodArgs_GetsPtr)
{
    Pool<Mock> allocator(1);
    auto pos = allocator.Alloc(TestArg);
    auto mock = Mock{TestArg};
    auto actual = allocator.GetPtrTo(pos);
    EXPECT_EQ(actual->val, mock.val);
    allocator.Free(pos);
}

TEST(Pool_unit_tests, GetPtrTo_BadArgs_Throws)
{
    Pool<Mock> allocator(1);
    EXPECT_THROW(allocator.GetPtrTo(0u), std::runtime_error);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}