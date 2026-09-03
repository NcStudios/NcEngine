#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/animation/BoneCache.h"
#include "ncutility/NcError.h"

#include <array>
#include <cstring>

TEST(BoneCacheTest, Allocate_consecutiveCalls_allocateContiguously)
{
    auto uut = nc::graphics::BoneCache{10};
    auto& staging = uut.GetStagingArea();

    const auto first = staging.Allocate(2);
    const auto second = staging.Allocate(3);
    EXPECT_EQ(0, first);
    EXPECT_EQ(2, second);
}

TEST(BoneCacheTest, Allocate_availableSlotInFreeList_allocatesFromList)
{
    auto uut = nc::graphics::BoneCache{10};
    auto& staging = uut.GetStagingArea();
    staging.Free(staging.Allocate(3));

    const auto handle = staging.Allocate(2);
    EXPECT_EQ(0, handle);
    const auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(2, freeList.at(0).index);
    EXPECT_EQ(1, freeList.at(0).capacity);
}

TEST(BoneCacheTest, Allocate_largerThanFreeSlot_allocatesFromEnd)
{
    auto uut = nc::graphics::BoneCache{10};
    auto& staging = uut.GetStagingArea();
    staging.Free(staging.Allocate(3));

    const auto handle = staging.Allocate(4);
    EXPECT_EQ(3, handle);
    const auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(0, freeList.at(0).index);
    EXPECT_EQ(3, freeList.at(0).capacity);
}

TEST(BoneCacheTest, Allocate_exceedsCapacity_throws)
{
    auto uut = nc::graphics::BoneCache{10};
    auto& staging = uut.GetStagingArea();
    EXPECT_THROW(staging.Allocate(11), nc::NcError);
}

TEST(BoneCacheTest, Free_mergesFreeListLeft)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();

    const auto first = staging.Allocate(2);
    const auto second = staging.Allocate(2);
    staging.Free(first);
    staging.Free(second);
    auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(0, freeList.at(0).index);
    EXPECT_EQ(4, freeList.at(0).capacity);
}

TEST(BoneCacheTest, Free_mergesFreeListRight)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();

    const auto first = staging.Allocate(2);
    const auto second = staging.Allocate(2);
    staging.Free(second);
    staging.Free(first);
    auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(0, freeList.at(0).index);
    EXPECT_EQ(4, freeList.at(0).capacity);
}

TEST(BoneCacheTest, Free_mergesFreeListLeftAndRight)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();

    const auto first = staging.Allocate(2);
    const auto second = staging.Allocate(2);
    const auto third = staging.Allocate(2);
    staging.Free(third);
    staging.Free(first);

    {
        auto& freeList = staging.GetFreeList();
        ASSERT_EQ(2, freeList.size());
    }

    staging.Free(second);
    auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(0, freeList.at(0).index);
    EXPECT_EQ(6, freeList.at(0).capacity);
}

TEST(BoneCacheTest, UpdateRegion_validCall_writesData)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    const auto handle = staging.Allocate(3);
    uut.CommitPendingChanges();

    const auto matrix = DirectX::XMMatrixScaling(1.0f, 2.0f, 3.0f);
    const auto data = nc::graphics::BoneData{matrix};
    const auto range = std::array{data, data, data};
    uut.UpdateRegion(handle, range);

    const auto [instances, _] = uut.BuildUpdateInfo();
    ASSERT_EQ(range.size(), instances.size());
    const auto sizeBytes = sizeof(nc::graphics::BoneData) * range.size();
    EXPECT_EQ(0, std::memcmp(range.data(), instances.data(), sizeBytes));
}

TEST(BoneCacheTest, UpdateRegion_outOfBounds_throws)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    const auto handle = staging.Allocate(2);
    uut.CommitPendingChanges();

    const auto data = nc::graphics::BoneData{};
    const auto range = std::array{data, data, data};
    EXPECT_THROW(uut.UpdateRegion(handle, range), nc::NcError);
}

TEST(BoneCacheTest, CommitPendingChanges_newAllocationsExceedCapacity_resizes)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    ASSERT_EQ(0, uut.GetCapacity());

    staging.Allocate(3);
    uut.CommitPendingChanges();
    EXPECT_EQ(3, uut.GetCapacity());

    staging.Allocate(1);
    uut.CommitPendingChanges();
    EXPECT_EQ(4, uut.GetCapacity());
}

TEST(BoneCacheTest, CommitPendingChanges_newAllocationsReuseSlots_preservesSize)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();

    const auto first = staging.Allocate(5);
    const auto second = staging.Allocate(5);
    uut.CommitPendingChanges();
    ASSERT_EQ(10, uut.GetCapacity());

    staging.Free(first);
    staging.Free(second);
    staging.Allocate(1);
    staging.Allocate(7);
    staging.Allocate(2);
    uut.CommitPendingChanges();
    EXPECT_EQ(10, uut.GetCapacity());
}

TEST(BoneCacheTest, BuildUpdateInfo_empty_returnsDefaultInfo)
{
    auto uut = nc::graphics::BoneCache{30};
    const auto actual = uut.BuildUpdateInfo();
    EXPECT_TRUE(actual.instances.empty());
    EXPECT_TRUE(actual.dirtyRanges.empty());
}

TEST(BoneCacheTest, BuildUpdateInfo_nonEmpty_returnsWholeBuffer)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    staging.Allocate(3);
    staging.Allocate(2);
    uut.CommitPendingChanges();

    const auto actual = uut.BuildUpdateInfo();
    EXPECT_EQ(5, actual.instances.size());
    EXPECT_EQ(1, actual.dirtyRanges.size());
    EXPECT_EQ(0, actual.dirtyRanges.at(0).offset);
    EXPECT_EQ(5, actual.dirtyRanges.at(0).count);
}

TEST(BoneCacheTest, Purge_noRemainingAllocations_resetsAll)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    const auto first = staging.Allocate(3);
    const auto second = staging.Allocate(2);
    uut.CommitPendingChanges();
    staging.Free(first);
    staging.Free(second);
    uut.CommitPendingChanges();
    uut.Purge();

    EXPECT_EQ(0, uut.GetCapacity());
    EXPECT_EQ(0, staging.GetCapacity());
    EXPECT_TRUE(staging.GetFreeList().empty());
}

TEST(BoneCacheTest, Purge_allocationPreservedAtFront_shrinks)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    staging.Allocate(1);
    const auto second = staging.Allocate(2);
    const auto third = staging.Allocate(3);
    uut.CommitPendingChanges();

    // preserving only the left most allocation should compact to the left
    staging.Free(second);
    staging.Free(third);
    uut.CommitPendingChanges();
    uut.Purge();
    EXPECT_EQ(1, uut.GetCapacity());
    EXPECT_EQ(1, staging.GetCapacity());
    EXPECT_TRUE(staging.GetFreeList().empty());
}

TEST(BoneCacheTest, Purge_allocationPreservedAtBack_maintainsFreeList)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    const auto first = staging.Allocate(1);
    const auto second = staging.Allocate(2);
    staging.Allocate(3);
    uut.CommitPendingChanges();

    // preserving the last allocation allows no compacting, except in the free list
    staging.Free(first);
    staging.Free(second);
    uut.CommitPendingChanges();
    uut.Purge();
    EXPECT_EQ(6, uut.GetCapacity());
    EXPECT_EQ(6, staging.GetCapacity());
    const auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(0, freeList.at(0).index);
    EXPECT_EQ(3, freeList.at(0).capacity);
}

TEST(BoneCacheTest, Purge_multiplePreservedAllocationsSplitAcrossBuffer_patchesFreeList)
{
    auto uut = nc::graphics::BoneCache{30};
    auto& staging = uut.GetStagingArea();
    staging.Allocate(1);
    const auto toRemove1 = staging.Allocate(2);
    const auto toRemove2 = staging.Allocate(3);
    staging.Allocate(4);
    uut.CommitPendingChanges();

    // preserving left and right most allocations allows no compacting, except in the free list
    staging.Free(toRemove1);
    staging.Free(toRemove2);
    uut.CommitPendingChanges();
    uut.Purge();
    EXPECT_EQ(10, uut.GetCapacity());
    EXPECT_EQ(10, staging.GetCapacity());
    const auto& freeList = staging.GetFreeList();
    ASSERT_EQ(1, freeList.size());
    EXPECT_EQ(1, freeList.at(0).index);
    EXPECT_EQ(5, freeList.at(0).capacity);
}
