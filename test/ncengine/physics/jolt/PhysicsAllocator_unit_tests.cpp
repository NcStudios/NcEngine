#include "gtest/gtest.h"
#include <vector>

auto s_allocations = std::vector<void*>{};

#define NC_CUSTOM_ALLOC_HOOKS
#define NC_ALLOC_HOOK(ptr, size) s_allocations.push_back(ptr);
#define NC_REALLOC_HOOK(ptr, size) NC_ALLOC_HOOK(ptr, size);
#define NC_ALIGNED_ALLOC_HOOK(ptr, size) NC_ALLOC_HOOK(ptr, size);
#define NC_FREE_HOOK(ptr) std::erase(s_allocations, ptr);
#define NC_ALIGNED_FREE_HOOK(ptr) NC_FREE_HOOK(ptr);

#include "physics/jolt/JoltApi.h"

TEST(PhysicsAllocatorTests, AllocateFunctions_succeed)
{
    const auto allocation = nc::physics::AllocateImpl(2u);
    EXPECT_EQ(1ull, s_allocations.size());
    const auto reallocation = nc::physics::ReallocateImpl(allocation, 2u, 4u);
    EXPECT_EQ(2ull, s_allocations.size());
    const auto alignedAllocation = nc::physics::AlignedAllocateImpl(4u, 16u);
    EXPECT_EQ(3ull, s_allocations.size());
    nc::physics::FreeImpl(reallocation);
    nc::physics::AlignedFreeImpl(alignedAllocation);
    s_allocations.clear();
}

TEST(PhysicsAllocatorTests, TempAllocator_AllocateFree_correctOrder_succeeds)
{
    auto uut = nc::physics::TempAllocator{512u};
    auto first = uut.Allocate(42u);
    auto second = uut.Allocate(1u);
    auto third = uut.Allocate(100u);
    uut.Free(third, 100u);
    uut.Free(second, 1u);
    uut.Free(first, 42u);
}

TEST(PhysicsAllocatorTests, TempAllocator_AllocateFree_outOfOrder_throws)
{
    auto uut = nc::physics::TempAllocator{512u};
    auto first = uut.Allocate(42u);
    uut.Allocate(1u);
    EXPECT_THROW(uut.Free(first, 42u), nc::NcError);
}

TEST(PhysicsAllocatorTests, TempAllocator_Allocate_outOfMemory_throws)
{
    auto uut = nc::physics::TempAllocator{512u};
    uut.Allocate(512u);
    EXPECT_THROW(uut.Allocate(1u), nc::NcError);
}

TEST(PhysicsAllocatorTests, TempAllocator_Free_wrongSize_throws)
{
    auto uut = nc::physics::TempAllocator{512u};
    auto first = uut.Allocate(42u);
    EXPECT_THROW(uut.Free(first, 2u), nc::NcError);
}

TEST(PhysicsAllocatorTests, TempAllocator_UtilityFunc_returnExpectedValues)
{
    auto uut = nc::physics::TempAllocator{512u};
    EXPECT_TRUE(uut.IsEmpty());
    EXPECT_EQ(512u, uut.GetSize());
    EXPECT_EQ(0u, uut.GetUsage());
    EXPECT_TRUE(uut.CanAllocate(512));
    EXPECT_FALSE(uut.CanAllocate(1024));

    auto first = uut.Allocate(128u);
    auto second = uut.Allocate(16u);
    constexpr auto expectedUsage = 128u + 16u;
    auto addressBefore = static_cast<char*>(first);
    --addressBefore; // one byte before begin of buffer
    auto addressAfter = static_cast<char*>(second);
    addressAfter += 512u; // one byte after end of buffer

    EXPECT_FALSE(uut.IsEmpty());
    EXPECT_EQ(512u, uut.GetSize());
    EXPECT_EQ(expectedUsage, uut.GetUsage());
    EXPECT_TRUE(uut.CanAllocate(512u - expectedUsage));
    EXPECT_FALSE(uut.CanAllocate(512u));
    EXPECT_TRUE(uut.OwnsMemory(first));
    EXPECT_TRUE(uut.OwnsMemory(second));
    EXPECT_FALSE(uut.OwnsMemory(addressBefore));
    EXPECT_FALSE(uut.OwnsMemory(addressAfter));
}
