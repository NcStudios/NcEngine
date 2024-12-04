#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/InstanceCache.h"

#include <array>

using TestInstanceCache = nc::graphics::InstanceCache<nc::graphics::StaticMeshInstanceData>;

constexpr auto g_mesh1 = nc::asset::MeshView{
    .id = 11111111,
    .firstVertex = 0,
    .vertexCount = 4,
    .firstIndex = 0,
    .indexCount = 6
};

constexpr auto g_mesh2 = nc::asset::MeshView{
    .id = 22222222,
    .firstVertex = 4,
    .vertexCount = 4,
    .firstIndex = 6,
    .indexCount = 6
};

constexpr auto g_mesh3 = nc::asset::MeshView{
    .id = 33333333,
    .firstVertex = 8,
    .vertexCount = 8,
    .firstIndex = 12,
    .indexCount = 24
};

constexpr auto g_allBatches = std::array{
    nc::MaterialPass::Shadow,
    nc::MaterialPass::Toon
};

struct TestObjectInfo
{
    uint32_t transformIndex;
    nc::MaterialInstanceHandle materialIndex;
    nc::MaterialPasses passes;
    nc::asset::MeshView mesh;
};

struct Batch1
{
    static constexpr auto passes = nc::MaterialPass::Toon;
    static constexpr auto& mesh = g_mesh1;
    static constexpr auto key = nc::graphics::BatchKey{passes, mesh.id};
    static constexpr auto objects = std::array{
        TestObjectInfo{0, 0, passes, mesh},
        TestObjectInfo{1, 1, passes, mesh},
        TestObjectInfo{2, 2, passes, mesh},
        TestObjectInfo{3, 3, passes, mesh},
        TestObjectInfo{4, 4, passes, mesh}
    };
};

struct Batch2
{
    static constexpr auto passes = nc::MaterialPass::Toon;
    static constexpr auto& mesh = g_mesh2;
    static constexpr auto key = nc::graphics::BatchKey{passes, mesh.id};
    static constexpr auto objects = std::array{
        TestObjectInfo{5, 5, passes, mesh},
        TestObjectInfo{6, 6, passes, mesh},
        TestObjectInfo{7, 7, passes, mesh}
    };
};

struct Batch3
{
    static constexpr auto passes = nc::MaterialPass::Shadow |
                                   nc::MaterialPass::Toon;
    static constexpr auto& mesh = g_mesh1;
    static constexpr auto key = nc::graphics::BatchKey{passes, mesh.id};
    static constexpr auto objects = std::array{
        TestObjectInfo{8, 8, passes, mesh},
        TestObjectInfo{9, 9, passes, mesh},
        TestObjectInfo{10, 10, passes, mesh}
    };
};

void AddTestInstance(TestInstanceCache& uut, uint32_t entityId, const TestObjectInfo& info)
{
    return uut.GetStagingArea().AddInstance(
        entityId,
        info.passes,
        info.mesh,
        nc::graphics::StaticMeshInstanceData{
            info.transformIndex,
            info.materialIndex
        }
    );
}

template<class BatchDesc>
void RemoveTestInstance(TestInstanceCache& uut, uint32_t entityId)
{
    uut.GetStagingArea().RemoveInstance(entityId, BatchDesc::passes, BatchDesc::mesh.id);
}

template<class BatchDesc>
void VerifyBatch(TestInstanceCache& uut,
                 uint32_t offset,
                 uint32_t count,
                 uint32_t capacity)
{
    const auto key = nc::graphics::BatchKey{BatchDesc::passes, BatchDesc::mesh.id};
    const auto& region = uut.GetRegion(key);
    EXPECT_EQ(offset, region.batch.firstInstance);
    EXPECT_EQ(count, region.batch.instanceCount);
    EXPECT_EQ(capacity, region.capacity);

    EXPECT_EQ(BatchDesc::mesh.firstIndex, region.batch.firstIndex);
    EXPECT_EQ(BatchDesc::mesh.indexCount, region.batch.indexCount);
    EXPECT_EQ(BatchDesc::mesh.firstVertex, region.batch.vertexOffset);
}

void VerifyInstance(const TestInstanceCache& uut,
                    uint32_t entityId,
                    uint32_t instanceIndex,
                    const TestObjectInfo& expectedInfo)
{
    EXPECT_TRUE(uut.IsValidInstance(entityId));
    EXPECT_EQ(instanceIndex, uut.GetInstanceIndex(entityId));
    EXPECT_EQ(entityId, uut.GetEntityId(instanceIndex));

    const auto& instanceData = uut.GetInstanceData(instanceIndex);
    EXPECT_EQ(instanceData.transformIndex, expectedInfo.transformIndex);
    EXPECT_EQ(instanceData.materialIndex, expectedInfo.materialIndex);
}

TEST(InstanceCacheTest, AddInstance_single)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto& info1 = Batch1::objects.at(0);
    AddTestInstance(uut, id1, info1);
    uut.CommitPendingChanges();

    EXPECT_EQ(1, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(1, uut.GetBatchCount());
    VerifyInstance(uut, id1, 0, info1);
    VerifyBatch<Batch1>(uut, 0, 1, 1);
}

TEST(InstanceCacheTest, AddInstance_multiple)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch1::objects.at(1);
    const auto& info3 = Batch1::objects.at(2);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    EXPECT_EQ(3, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(1, uut.GetBatchCount());
    VerifyInstance(uut, id1, 0, info1);
    VerifyInstance(uut, id2, 1, info2);
    VerifyInstance(uut, id3, 2, info3);
    VerifyBatch<Batch1>(uut, 0, 3, 3);
}

TEST(InstanceCacheTests, AddInstance_multipleBatches)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto id4 = 3;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    const auto& info4 = Batch3::objects.at(1);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    AddTestInstance(uut, id4, info4);
    uut.CommitPendingChanges();

    EXPECT_EQ(4, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(3, uut.GetBatchCount());
    VerifyInstance(uut, id1, 0, info1);
    VerifyInstance(uut, id2, 1, info2);
    VerifyInstance(uut, id3, 2, info3);
    VerifyInstance(uut, id4, 3, info4);
    VerifyBatch<Batch1>(uut, 0, 1, 1);
    VerifyBatch<Batch2>(uut, 1, 1, 1);
    VerifyBatch<Batch3>(uut, 2, 2, 2);
}

TEST(InstanceCacheTests, AddInstance_hasSubsequentBatches)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    const auto id4 = 3;
    const auto& info4 = Batch1::objects.at(1);
    AddTestInstance(uut, id4, info4);
    uut.CommitPendingChanges();

    // Initial State:
    // | B1 | B2 | B3 |
    //  id1, id2, id3
    //
    // State After Insertion:
    // |    B1   | B2 | B3 |
    //  id1, id4, id2, id3

    EXPECT_EQ(4, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(3, uut.GetBatchCount());
    VerifyInstance(uut, id1, 0, info1);
    VerifyInstance(uut, id4, 1, info4);
    VerifyInstance(uut, id2, 2, info2);
    VerifyInstance(uut, id3, 3, info3);
    VerifyBatch<Batch1>(uut, 0, 2, 2);
    VerifyBatch<Batch2>(uut, 2, 1, 1);
    VerifyBatch<Batch3>(uut, 3, 1, 1);
}

TEST(InstanceCacheTests, Remove)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch2>(uut, id2);
    RemoveTestInstance<Batch3>(uut, id3);
    uut.CommitPendingChanges();

    // Initial State:
    // | B1 | B2 | B3 |
    //  id1, id2, id3
    //
    // State After Removals:
    // | B1 | B2 | B3 |
    //   NA,  NA,  NA

    EXPECT_EQ(3, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(3, uut.GetBatchCount());
    EXPECT_FALSE(uut.IsValidInstance(id1));
    EXPECT_FALSE(uut.IsValidInstance(id2));
    EXPECT_FALSE(uut.IsValidInstance(id3));
    VerifyBatch<Batch1>(uut, 0, 0, 1);
    VerifyBatch<Batch2>(uut, 1, 0, 1);
    VerifyBatch<Batch3>(uut, 2, 0, 1);
}

TEST(InstanceCacheTests, InsertAfterRemove)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch1::objects.at(1);
    const auto& info3 = Batch2::objects.at(2);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch1>(uut, id2);
    RemoveTestInstance<Batch2>(uut, id3);
    uut.CommitPendingChanges();

    AddTestInstance(uut, id1, info1);
    uut.CommitPendingChanges();

    // Initial State:
    // |   B1    | B2 |
    //  id1, id2, id3
    //
    // State After Removals + Insertion:
    // |    B1   | B2 |
    //   id1, NA,  NA

    EXPECT_EQ(3, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(2, uut.GetBatchCount());
    EXPECT_FALSE(uut.IsValidInstance(id2));
    EXPECT_FALSE(uut.IsValidInstance(id3));
    VerifyInstance(uut, id1, 0, info1);
    VerifyBatch<Batch1>(uut, 0, 1, 2);
    VerifyBatch<Batch2>(uut, 2, 0, 1);
}

TEST(InstanceCacheTests, Purge_clearAll)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch2>(uut, id2);
    RemoveTestInstance<Batch3>(uut, id3);
    uut.Purge();

    // Expect totally empty state
    EXPECT_EQ(0, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(0, uut.GetBatchCount());
    EXPECT_FALSE(uut.IsValidInstance(id1));
    EXPECT_FALSE(uut.IsValidInstance(id2));
    EXPECT_FALSE(uut.IsValidInstance(id3));
    EXPECT_FALSE(uut.HasBatchFor(Batch1::key));
    EXPECT_FALSE(uut.HasBatchFor(Batch2::key));
    EXPECT_FALSE(uut.HasBatchFor(Batch3::key));
}

TEST(InstanceCacheTests, Purge_preservesPersistingObjects)
{
    const auto batchSize = 2u;
    auto uut = TestInstanceCache{32, batchSize};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto id4 = 3;
    const auto id5 = 4;
    const auto id6 = 5;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch2::objects.at(1);
    const auto& info4 = Batch2::objects.at(2);
    const auto& info5 = Batch3::objects.at(0);
    const auto& info6 = Batch3::objects.at(1);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    AddTestInstance(uut, id4, info4);
    AddTestInstance(uut, id5, info5);
    AddTestInstance(uut, id6, info6);
    uut.CommitPendingChanges();

    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch2>(uut, id2);
    RemoveTestInstance<Batch2>(uut, id4);
    RemoveTestInstance<Batch3>(uut, id6);
    uut.Purge();

    // id3 & id5 are kept, which should preserve Batch2 & Batch3, compacting them towards the front
    // Initial State:
    // |   B1   |      B2      |   B3   |
    //  id1, NA, id2, id3, id4, id5, id6
    //
    // State After Purge:
    // |   B2   |   B3  |
    //  id3, NA, id5, NA
    EXPECT_EQ(batchSize * 2, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(2, uut.GetBatchCount());
    VerifyInstance(uut, id3, 0, info3);
    VerifyInstance(uut, id5, batchSize, info5);
    VerifyBatch<Batch2>(uut, 0, 1, batchSize);
    VerifyBatch<Batch3>(uut, batchSize, 1, batchSize);

    EXPECT_FALSE(uut.IsValidInstance(id1));
    EXPECT_FALSE(uut.IsValidInstance(id2));
    EXPECT_FALSE(uut.IsValidInstance(id4));
    EXPECT_FALSE(uut.IsValidInstance(id6));
    EXPECT_FALSE(uut.HasBatchFor(Batch1::key));
}

TEST(InstanceCacheTests, AddInstance_afterClear)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto id4 = 3;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch2::objects.at(1);
    const auto& info4 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    AddTestInstance(uut, id4, info4);
    uut.CommitPendingChanges();

    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch2>(uut, id2);
    RemoveTestInstance<Batch3>(uut, id4);
    uut.Purge();

    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    uut.CommitPendingChanges();

    // id3 is kept preserving Batch2. id1 & id2 are then added back
    // Initial State:
    // | B1 |   B2    | B3 |
    //  id1, id2, id3, id4
    //
    // State After Purge + Readd:
    // |   B2    | B1 |
    //  id3, id2,  id1

    EXPECT_EQ(3, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(2, uut.GetBatchCount());
    VerifyInstance(uut, id3, 0, info3);
    VerifyInstance(uut, id2, 1, info2);
    VerifyInstance(uut, id1, 2, info1);
    VerifyBatch<Batch2>(uut, 0, 2, 2);
    VerifyBatch<Batch1>(uut, 2, 1, 1);

    EXPECT_FALSE(uut.IsValidInstance(id4));
    EXPECT_FALSE(uut.HasBatchFor(Batch3::key));
}

TEST(InstanceCacheTests, UpdateInstance)
{
    auto uut = TestInstanceCache{32};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    const auto& newInfo1 = Batch3::objects.at(1);
    const auto& newInfo2 = Batch3::objects.at(2);

    uut.GetStagingArea().UpdateInstance(
        id1,
        info1.passes,
        newInfo1.passes,
        info1.mesh.id,
        newInfo1.mesh,
        nc::graphics::StaticMeshInstanceData{
            newInfo1.transformIndex,
            newInfo1.materialIndex
        }
    );

    uut.GetStagingArea().UpdateInstance(
        id2,
        info2.passes,
        newInfo2.passes,
        info2.mesh.id,
        newInfo2.mesh,
        nc::graphics::StaticMeshInstanceData{
            newInfo2.transformIndex,
            newInfo2.materialIndex
        }
    );

    uut.CommitPendingChanges();

    // id1 & id2 moved to Batch3
    // Initial State:
    // | B1  | B2  | B3 |
    //   id1,  id2,  id3
    //
    // State After Update:
    // | B1 | B2 |      B3      |
    //   NA,  NA,  id3, id1, id2

    EXPECT_EQ(5, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(3, uut.GetBatchCount());
    VerifyInstance(uut, id3, 2, info3);
    VerifyInstance(uut, id1, 3, newInfo1);
    VerifyInstance(uut, id2, 4, newInfo2);
    VerifyBatch<Batch1>(uut, 0, 0, 1);
    VerifyBatch<Batch2>(uut, 1, 0, 1);
    VerifyBatch<Batch3>(uut, 2, 3, 3);
}

TEST(InstanceCacheTests, BuildState)
{
    auto uut = TestInstanceCache{32};

    // empty state - not dirty
    {
        auto actual = uut.BuildState();
        EXPECT_TRUE(actual.instances.empty());
        EXPECT_TRUE(actual.dirtyRanges.empty());
    }

    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto id4 = 3;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch1::objects.at(1);
    const auto& info3 = Batch2::objects.at(0);
    const auto& info4 = Batch2::objects.at(1);

    // add batch - whole batch dirty
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildState();
        EXPECT_FALSE(actual.instances.empty());
        ASSERT_EQ(1, actual.dirtyRanges.size());
        EXPECT_EQ(0, actual.dirtyRanges[0].offset);
        EXPECT_EQ(2, actual.dirtyRanges[0].count);
    }

    // add subsequent batch batch - dirty from new batch to end
    AddTestInstance(uut, id3, info3);
    AddTestInstance(uut, id4, info4);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildState();
        EXPECT_FALSE(actual.instances.empty());
        ASSERT_EQ(1, actual.dirtyRanges.size());
        EXPECT_EQ(2, actual.dirtyRanges[0].offset);
        EXPECT_EQ(2, actual.dirtyRanges[0].count);
    }

    // no changes - not dirty
    {
        auto actual = uut.BuildState();
        EXPECT_TRUE(actual.instances.empty());
        EXPECT_TRUE(actual.dirtyRanges.empty());
    }

    // last item in batch changed - not dirty
    RemoveTestInstance<Batch1>(uut, id2);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildState();
        EXPECT_TRUE(actual.instances.empty());
        EXPECT_TRUE(actual.dirtyRanges.empty());
    }

    // last batch changed - dirty from modified to end
    RemoveTestInstance<Batch2>(uut, id3);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildState();
        EXPECT_FALSE(actual.instances.empty());
        ASSERT_EQ(1, actual.dirtyRanges.size());
        EXPECT_EQ(2, actual.dirtyRanges[0].offset);
        EXPECT_EQ(2, actual.dirtyRanges[0].count);
    }

    // multiple batches changed - dirty from leftmost batch to end
    RemoveTestInstance<Batch1>(uut, id1);
    RemoveTestInstance<Batch2>(uut, id4);
    AddTestInstance(uut, id1, info1);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildState();
        EXPECT_FALSE(actual.instances.empty());
        ASSERT_EQ(1, actual.dirtyRanges.size());
        EXPECT_EQ(0, actual.dirtyRanges[0].offset);
        EXPECT_EQ(4, actual.dirtyRanges[0].count);
    }

    // after purge with item remaining - dirty from begin to end
    uut.Purge();
    {
        auto actual = uut.BuildState();
        EXPECT_FALSE(actual.instances.empty());
        ASSERT_EQ(1, actual.dirtyRanges.size());
        EXPECT_EQ(0, actual.dirtyRanges[0].offset);
        EXPECT_EQ(1, actual.dirtyRanges[0].count);
    }

    // after purge all - not dirty
    RemoveTestInstance<Batch1>(uut, id1);
    uut.Purge();
    {
        auto actual = uut.BuildState();
        EXPECT_TRUE(actual.instances.empty());
        EXPECT_TRUE(actual.dirtyRanges.empty());
    }
}

TEST(InstanceCacheTests, BuildBatches)
{
    auto uut = TestInstanceCache{32, 100};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0); // toon
    const auto& info2 = Batch2::objects.at(0); // toon
    const auto& info3 = Batch3::objects.at(0); // shader | toon
    constexpr auto batchCount = g_allBatches.size();

    // no batches - all passes empty
    {
        auto actual = uut.BuildBatches(g_allBatches);
        EXPECT_EQ(batchCount, actual.size());
        for (const auto& pass : actual)
        {
            EXPECT_TRUE(pass.empty());
        }
    }

    // has batches - batches reported across all associated passes
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildBatches(g_allBatches);
        EXPECT_EQ(batchCount, actual.size());
        EXPECT_EQ(1, actual.at(0).size()); // shadow
        EXPECT_EQ(3, actual.at(1).size()); // toon
    }

    // has empty batch - not reported in passes
    RemoveTestInstance<Batch3>(uut, id3);
    uut.CommitPendingChanges();
    uut.CommitPendingChanges();
    {
        auto actual = uut.BuildBatches(g_allBatches);
        EXPECT_EQ(batchCount, actual.size());
        EXPECT_EQ(0, actual.at(0).size()); // shadow
        EXPECT_EQ(2, actual.at(1).size()); // toon
    }
}

TEST(InstanceCacheTests, LargerInitialBatchSize)
{
    auto uut = TestInstanceCache{32, 100};
    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;
    const auto& info1 = Batch1::objects.at(0);
    const auto& info2 = Batch2::objects.at(0);
    const auto& info3 = Batch3::objects.at(0);
    AddTestInstance(uut, id1, info1);
    AddTestInstance(uut, id2, info2);
    AddTestInstance(uut, id3, info3);
    uut.CommitPendingChanges();

    EXPECT_EQ(300, uut.GetInstanceCountUpperBound());
    EXPECT_EQ(3, uut.GetBatchCount());
    VerifyInstance(uut, id1, 0, info1);
    VerifyInstance(uut, id2, 100, info2);
    VerifyInstance(uut, id3, 200, info3);
    VerifyBatch<Batch1>(uut, 0, 1, 100);
    VerifyBatch<Batch2>(uut, 100, 1, 100);
    VerifyBatch<Batch3>(uut, 200, 1, 100);
}
