#include "gtest/gtest.h"
#include "alloc/Allocator.h"
#include <cstdint>

using namespace nc::alloc;

template<class T>
class NewDeleteResource
{
    public:
        using value_type = T;
        NewDeleteResource(size_t) {}
        value_type* allocate(size_t) { return new T; }
        void deallocate(value_type* ptr, size_t) { delete ptr; }
        void free_all() {};
};

class FixedTypeAllocator_unit_tests : public ::testing::Test
{
    protected:
        void SetUp() override { allocator.initialize_memory_resource(0u); }
        void TearDown() override { allocator.release_memory_resource(); }
    
    public:
        using value_type = int;
        FixedTypeAllocator<NewDeleteResource<value_type>> allocator;
};

TEST_F(FixedTypeAllocator_unit_tests, InitializeMemoryResource_AlreadyInitialized_Throws)
{
    EXPECT_THROW(allocator.initialize_memory_resource(0u), std::runtime_error);
}

TEST_F(FixedTypeAllocator_unit_tests, Allocate_ValidCall_ReturnsNonNull)
{
    auto actual = allocator.allocate(1u);
    EXPECT_NE(actual, nullptr);
    allocator.deallocate(actual, 1u);
}

TEST_F(FixedTypeAllocator_unit_tests, Allocate_ValidCall_ReturnsAlignedAddress)
{
    auto ptr = allocator.allocate(1u);
    auto actual = reinterpret_cast<std::uintptr_t>(ptr) % alignof(value_type);
    EXPECT_EQ(actual, 0u);
    allocator.deallocate(ptr, 1u);
}

TEST_F(FixedTypeAllocator_unit_tests, OperatorEqual_SameResourceType_ReturnsTrue)
{
    FixedTypeAllocator<NewDeleteResource<value_type>> differentAllocator;
    auto actual = allocator == differentAllocator;
    EXPECT_EQ(actual, true);
}

TEST_F(FixedTypeAllocator_unit_tests, OperatorEqual_DifferentResourceType_ReturnsFalse)
{
    FixedTypeAllocator<NewDeleteResource<float>> differentAllocator;
    auto actual = allocator == differentAllocator;
    EXPECT_EQ(actual, false);
}

TEST_F(FixedTypeAllocator_unit_tests, OperatorNotEqual_SameResourceType_ReturnsFalse)
{
    FixedTypeAllocator<NewDeleteResource<value_type>> differentAllocator;
    auto actual = allocator != differentAllocator;
    EXPECT_EQ(actual, false);
}

TEST_F(FixedTypeAllocator_unit_tests, OperatorNotEqual_DifferentResourceType_ReturnsTrue)
{
    FixedTypeAllocator<NewDeleteResource<float>> differentAllocator;
    auto actual = allocator != differentAllocator;
    EXPECT_EQ(actual, true);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}