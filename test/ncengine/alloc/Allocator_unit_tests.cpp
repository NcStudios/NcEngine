#include "gtest/gtest.h"
#include "alloc/Allocator.h"
#include <cstdint>

using namespace nc::alloc;

struct FakeTag {};

class PoolAllocator_unit_tests : public ::testing::Test
{
    public:
        const size_t resourceSize = 8u;
        using value_type = int;
        using allocator_type = PoolAllocator<value_type>;
        std::unique_ptr<allocator_type> allocator;

        using alt_allocator_type = PoolAllocator<value_type, FakeTag>;
        std::unique_ptr<alt_allocator_type> altAllocator;

    protected:
        void SetUp() override
        {
            allocator_type::create_memory_resource(resourceSize);
            alt_allocator_type::create_memory_resource(resourceSize);
            allocator = std::make_unique<allocator_type>();
            altAllocator = std::make_unique<alt_allocator_type>();
        }

        void TearDown() override
        {
            allocator_type::destroy_memory_resource();
            alt_allocator_type::destroy_memory_resource();
            allocator = nullptr;
            altAllocator = nullptr;
        }
};

class LinearAllocator_unit_tests : public ::testing::Test
{
    public:
        const size_t resourceSize = 8u;
        using value_type = int;
        using allocator_type = LinearAllocator<value_type>;
        std::unique_ptr<allocator_type> allocator;

        using alt_value_type = float;
        using alt_allocator_type = LinearAllocator<alt_value_type>;
        std::unique_ptr<alt_allocator_type> altAllocator;

    protected:
        void SetUp() override
        {
            allocator_type::create_memory_resource(resourceSize);
            allocator = std::make_unique<allocator_type>();
            altAllocator = std::make_unique<alt_allocator_type>();
        }

        void TearDown() override
        {
            allocator_type::destroy_memory_resource();
            allocator = nullptr;
            altAllocator = nullptr;
        }
};

TEST_F(PoolAllocator_unit_tests, InitializeMemoryResource_AlreadyInitialized_Throws)
{
    EXPECT_THROW(allocator_type::create_memory_resource(0u), std::runtime_error);
}

TEST_F(PoolAllocator_unit_tests, Allocate_ValidCall_ReturnsNonNull)
{
    auto actual = allocator->allocate(1u);
    EXPECT_NE(actual, nullptr);
    allocator->deallocate(actual, 1u);
}

TEST_F(PoolAllocator_unit_tests, Allocate_ValidCall_ReturnsAlignedAddress)
{
    auto ptr = allocator->allocate(1u);
    auto actual = reinterpret_cast<std::uintptr_t>(ptr) % alignof(value_type);
    EXPECT_EQ(actual, 0u);
    allocator->deallocate(ptr, 1u);
}

TEST_F(PoolAllocator_unit_tests, OperatorEqual_SameResourceType_ReturnsTrue)
{
    allocator_type sameTypeAllocator;
    auto actual = *allocator == sameTypeAllocator;
    EXPECT_EQ(actual, true);
}

TEST_F(PoolAllocator_unit_tests, OperatorEqual_DifferentResourceType_ReturnsFalse)
{
    auto actual = *allocator == *altAllocator;
    EXPECT_EQ(actual, false);
}

TEST_F(PoolAllocator_unit_tests, OperatorNotEqual_SameResourceType_ReturnsFalse)
{
    allocator_type sameTypeAllocator;
    auto actual = *allocator != sameTypeAllocator;
    EXPECT_EQ(actual, false);
}

TEST_F(PoolAllocator_unit_tests, OperatorNotEqual_DifferentResourceType_ReturnsTrue)
{
    auto actual = *allocator != *altAllocator;
    EXPECT_EQ(actual, true);
}

TEST_F(LinearAllocator_unit_tests, InitializeMemoryResource_AlreadyInitialized_Throws)
{
    EXPECT_THROW(allocator_type::create_memory_resource(0u), std::runtime_error);
}

TEST_F(LinearAllocator_unit_tests, Allocate_ValidCall_ReturnsNonNull)
{
    auto actual = allocator->allocate(1u);
    EXPECT_NE(actual, nullptr);
    allocator->deallocate(actual, 1u);
}

TEST_F(LinearAllocator_unit_tests, Allocate_ValidCall_ReturnsAlignedAddress)
{
    auto ptr = allocator->allocate(1u);
    auto actual = reinterpret_cast<std::uintptr_t>(ptr) % alignof(value_type);
    EXPECT_EQ(actual, 0u);
    allocator->deallocate(ptr, 1u);
}

TEST_F(LinearAllocator_unit_tests, Allocate_CallAfterClear_AllocatesFromBeginning)
{
    auto expected = allocator->allocate(1u);
    allocator->clear_memory_resource();
    auto actual = allocator->allocate(1u);
    EXPECT_EQ(actual, expected);
}

TEST_F(LinearAllocator_unit_tests, Allocate_FromDifferentType_AllocatesFromSameResource)
{
    auto expected = (void*)allocator->allocate(1u);
    allocator->clear_memory_resource();
    auto actual = (void*)altAllocator->allocate(1u);
    EXPECT_EQ(actual, expected);
}

TEST_F(LinearAllocator_unit_tests, OperatorEqual_SameResourceType_ReturnsTrue)
{
    allocator_type sameTypeAllocator;
    auto actual = *allocator == sameTypeAllocator;
    EXPECT_EQ(actual, true);
}

TEST_F(LinearAllocator_unit_tests, OperatorEqual_DifferentResourceType_ReturnsTrue)
{
    auto actual = *allocator == *altAllocator;
    EXPECT_EQ(actual, true);
}

TEST_F(LinearAllocator_unit_tests, OperatorNotEqual_SameResourceType_ReturnsFalse)
{
    allocator_type sameTypeAllocator;
    auto actual = *allocator != sameTypeAllocator;
    EXPECT_EQ(actual, false);
}

TEST_F(LinearAllocator_unit_tests, OperatorNotEqual_DifferentResourceType_ReturnsFalse)
{
    auto actual = *allocator != *altAllocator;
    EXPECT_EQ(actual, false);
}
int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}