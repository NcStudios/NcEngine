#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/HostStructuredBuffer.h"

#include <algorithm>

struct TestObject
{
    float value = 0.0f;
};

TEST(HostStructuredBufferTests, HostStructuredBufferStaging_HandleAllocation)
{
    auto uut = nc::graphics::HostStructuredBufferStaging<TestObject>(10);

    // handles are consecutive indices
    const auto first = uut.Emplace();
    const auto second = uut.Emplace();
    const auto third = uut.Emplace();
    EXPECT_EQ(0, first);
    EXPECT_EQ(1, second);
    EXPECT_EQ(2, third);

    // erased handles are reused
    uut.Erase(first);
    const auto recycled = uut.Emplace();
    EXPECT_EQ(0, recycled);

    // consecutive again
    const auto fourth = uut.Emplace();
    EXPECT_EQ(3, fourth);

    // staging should affect indices
    uut.ClearStaged();
    const auto fifth = uut.Emplace();
    EXPECT_EQ(4, fifth);

    // clear resets indices, including free list
    uut.Erase(fourth);
    uut.Erase(fifth);
    uut.Clear();
    const auto recycledFirst = uut.Emplace();
    EXPECT_EQ(0, recycledFirst);

    // update does not affect indices
    uut.Update(recycledFirst, TestObject{});
    const auto recycledSecond = uut.Emplace();
    EXPECT_EQ(1, recycledSecond);
}

TEST(HostStructuredBufferTests, HostStructuredBufferStaging_StagingBounds)
{
    const auto getLargestIndex = [](const auto& staged)
    {
        constexpr auto proj = &nc::graphics::StagedBufferItem<TestObject>::index;
        return std::ranges::max(staged, std::less{}, proj).index;
    };

    auto uut = nc::graphics::HostStructuredBufferStaging<TestObject>(10);

    // initially empty (bounds are past-the-end index)
    EXPECT_EQ(1, uut.GetStagedBounds());
    EXPECT_EQ(0, uut.GetStaged().size());

    // consecutive allocation reports largest
    const auto first = uut.Emplace();
    const auto second = uut.Emplace();
    const auto third = uut.Emplace();
    {
        const auto staged = uut.GetStaged();
        const auto bounds = uut.GetStagedBounds();
        const auto largestStaged = getLargestIndex(staged);
        EXPECT_EQ(3, staged.size());
        EXPECT_EQ(3, bounds);
        EXPECT_EQ(third, largestStaged);
        EXPECT_EQ(bounds, largestStaged + 1);
    }

    // removal doesn't affect staging
    uut.ClearStaged();
    uut.Erase(second);
    EXPECT_EQ(1, uut.GetStagedBounds());
    EXPECT_EQ(0, uut.GetStaged().size());

    // recycled indices included in bounds
    const auto recycledSecond = uut.Emplace(); // reuse index 1
    uut.Erase(first);
    uut.Emplace(); // reuse index 0
    {
        const auto staged = uut.GetStaged();
        const auto bounds = uut.GetStagedBounds();
        const auto largestStaged = getLargestIndex(staged);
        EXPECT_EQ(2, staged.size());
        EXPECT_EQ(2, bounds);
        EXPECT_EQ(recycledSecond, largestStaged);
        EXPECT_EQ(bounds, largestStaged + 1);
    }

    // recycled index plus larger non-recycled index reports largest
    uut.ClearStaged();
    uut.Erase(third);
    uut.Emplace(); // reuse index 2
    const auto fourth = uut.Emplace(); // new index 3
    {
        const auto staged = uut.GetStaged();
        const auto bounds = uut.GetStagedBounds();
        const auto largestStaged = getLargestIndex(staged);
        EXPECT_EQ(2, staged.size());
        EXPECT_EQ(4, bounds);
        EXPECT_EQ(fourth, largestStaged);
        EXPECT_EQ(bounds, largestStaged + 1);
    }

    // updated index included in bounds
    uut.ClearStaged();
    uut.Update(fourth, TestObject{});
    {
        const auto staged = uut.GetStaged();
        const auto bounds = uut.GetStagedBounds();
        const auto largestStaged = getLargestIndex(staged);
        EXPECT_EQ(1, staged.size());
        EXPECT_EQ(4, bounds);
        EXPECT_EQ(fourth, largestStaged);
        EXPECT_EQ(bounds, largestStaged + 1);
    }
}

TEST(HostStructuredBufferTests, CommitPendingChanges)
{
    auto uut = nc::graphics::HostStructuredBuffer<TestObject>(10);
    auto& staging = uut.GetStagingArea();

    // initial add resizes and marks modified indices dirty
    const auto first = staging.Emplace();
    const auto second = staging.Emplace();
    uut.CommitPendingChanges();
    {
        const auto dirty = uut.GetDirtyIndices();
        EXPECT_EQ(2, uut.size());
        ASSERT_EQ(2, dirty.size());
        EXPECT_EQ(first, dirty[0]);
        EXPECT_EQ(second, dirty[1]);
    }

    // modify within existing buffer capacity retains size and marks dirty
    uut.ClearDirtyIndices();
    staging.Erase(first);
    const auto replacedFirst = staging.Emplace();
    uut.CommitPendingChanges();
    {
        const auto dirty = uut.GetDirtyIndices();
        EXPECT_EQ(2, uut.size());
        ASSERT_EQ(1, dirty.size());
        EXPECT_EQ(replacedFirst, dirty[0]);
    }

    // exceeds buffer capacity resizes and marks dirty
    uut.ClearDirtyIndices();
    const auto third = staging.Emplace();
    uut.CommitPendingChanges();
    {
        const auto dirty = uut.GetDirtyIndices();
        EXPECT_EQ(3, uut.size());
        ASSERT_EQ(1, dirty.size());
        EXPECT_EQ(third, dirty[0]);
    }

    // update existing value retains size and marks dirty
    uut.ClearDirtyIndices();
    staging.Update(third, TestObject{});
    uut.CommitPendingChanges();
    {
        const auto dirty = uut.GetDirtyIndices();
        EXPECT_EQ(3, uut.size());
        ASSERT_EQ(1, dirty.size());
        EXPECT_EQ(third, dirty[0]);
    }
}

TEST(HostStructuredBufferTests, BuildUpdateInfo)
{
    auto uut = nc::graphics::HostStructuredBuffer<TestObject>(10);
    auto& staging = uut.GetStagingArea();

    // empty returns default info
    {
        auto info = uut.BuildUpdateInfo();
        EXPECT_TRUE(info.instances.empty());
        EXPECT_TRUE(info.dirtyRanges.empty());
    }

    // single range returns on modified range
    const auto first = staging.Emplace();
    const auto second = staging.Emplace();
    const auto third = staging.Emplace();
    uut.CommitPendingChanges();
    {
        auto info = uut.BuildUpdateInfo();
        EXPECT_EQ(3, info.instances.size());
        ASSERT_EQ(1, info.dirtyRanges.size());
        const auto& range = info.dirtyRanges.at(0);
        EXPECT_EQ(0, range.offset);
        EXPECT_EQ(3, range.count);
    }

    // removals not considered dirty
    staging.Erase(first);
    staging.Erase(third);
    uut.CommitPendingChanges();
    {
        auto info = uut.BuildUpdateInfo();
        EXPECT_TRUE(info.instances.empty());
        EXPECT_TRUE(info.dirtyRanges.empty());
    }

    // non-consecutive modifications reported as multiple ranges
    staging.Emplace();
    staging.Emplace();
    uut.CommitPendingChanges();
    {
        auto info = uut.BuildUpdateInfo();
        EXPECT_EQ(3, info.instances.size());
        ASSERT_EQ(2, info.dirtyRanges.size());
        const auto& range1 = info.dirtyRanges.at(0);
        const auto& range2 = info.dirtyRanges.at(1);
        EXPECT_EQ(0, range1.offset);
        EXPECT_EQ(1, range1.count);
        EXPECT_EQ(2, range2.offset);
        EXPECT_EQ(1, range2.count);
    }

    // updated values reported dirty
    staging.Update(first, TestObject{});
    staging.Update(second, TestObject{});
    uut.CommitPendingChanges();
    {
        auto info = uut.BuildUpdateInfo();
        EXPECT_EQ(3, info.instances.size());
        ASSERT_EQ(1, info.dirtyRanges.size());
        const auto& range = info.dirtyRanges.at(0);
        EXPECT_EQ(0, range.offset);
        EXPECT_EQ(2, range.count);
    }
}

TEST(HostStructuredBufferTests, AccessForWrite)
{
    auto uut = nc::graphics::HostStructuredBuffer<TestObject>(10);
    auto& staging = uut.GetStagingArea();
    const auto index = staging.Emplace();
    uut.CommitPendingChanges();
    uut.ClearDirtyIndices();
    ASSERT_TRUE(uut.GetDirtyIndices().empty());

    uut.AccessForWrite(index).value = 100.0f;
    const auto dirty = uut.GetDirtyIndices();
    ASSERT_EQ(1, dirty.size());
    EXPECT_EQ(index, dirty[0]);
    EXPECT_EQ(100.0f, uut.AccessForRead(index).value);
}

TEST(HostStructuredBufferTests, SortDirtyIndices)
{
    auto uut = nc::graphics::HostStructuredBuffer<TestObject>(10);
    auto& staging = uut.GetStagingArea();
    const auto first = staging.Emplace();
    const auto second = staging.Emplace();
    const auto third = staging.Emplace();
    const auto fourth = staging.Emplace();
    uut.CommitPendingChanges();
    uut.ClearDirtyIndices();

    uut.MarkDirty(third);
    uut.MarkDirty(third);
    uut.MarkDirty(first);
    uut.MarkDirty(fourth);
    uut.MarkDirty(second);
    uut.MarkDirty(first);
    uut.MarkDirty(third);
    ASSERT_EQ(7, uut.GetDirtyIndices().size());

    uut.SortDirtyIndices();
    const auto dirty = uut.GetDirtyIndices();
    ASSERT_EQ(4, dirty.size());
    EXPECT_EQ(first, dirty[0]);
    EXPECT_EQ(second, dirty[1]);
    EXPECT_EQ(third, dirty[2]);
    EXPECT_EQ(fourth, dirty[3]);
}
