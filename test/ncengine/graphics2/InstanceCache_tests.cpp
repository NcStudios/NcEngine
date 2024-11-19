#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/InstanceCache.h"

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
    nc::MaterialPass::Toon,
    nc::MaterialPass::Alpha
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
    static constexpr auto objects = std::array{
        TestObjectInfo{0, 0, passes, mesh},
        TestObjectInfo{5, 3, passes, mesh},
        TestObjectInfo{7, 2, passes, mesh}
    };
};

struct Batch2
{
    static constexpr auto passes = nc::MaterialPass::Toon;
    static constexpr auto& mesh = g_mesh2;
    static constexpr auto objects = std::array{
        TestObjectInfo{9, 1, passes, mesh},
        TestObjectInfo{1, 4, passes, mesh},
        TestObjectInfo{8, 8, passes, mesh}
    };
};

struct Batch3
{
    static constexpr auto passes = nc::MaterialPass::Shadow |
                                   nc::MaterialPass::Toon |
                                   nc::MaterialPass::Alpha;
    static constexpr auto& mesh = g_mesh1;
    static constexpr auto objects = std::array{
        TestObjectInfo{2, 7, passes, mesh},
        TestObjectInfo{4, 5, passes, mesh},
        TestObjectInfo{3, 9, passes, mesh}
    };
};

void AddTestInstance(nc::graphics::InstanceCache2& uut, uint32_t entityId, const TestObjectInfo& info)
{
    return uut.AddInstance(
        entityId,
        info.transformIndex,
        info.materialIndex,
        info.passes,
        info.mesh
    );
}

template<class BatchDesc>
void VerifyBatch(nc::graphics::InstanceCache2& uut,
                 uint32_t offset,
                 uint32_t count,
                 uint32_t capacity)
{
    const auto& batch = uut.GetBatch(BatchDesc::mesh.id, BatchDesc::passes);
    EXPECT_EQ(offset, batch.offset);
    EXPECT_EQ(count, batch.count);
    EXPECT_EQ(capacity, batch.capacity);

    EXPECT_EQ(BatchDesc::mesh.firstIndex, batch.indexOffset);
    EXPECT_EQ(BatchDesc::mesh.indexCount, batch.indexCount);
    EXPECT_EQ(BatchDesc::mesh.firstVertex, batch.vertexOffset);
}

// constexpr auto operator==(const nc::graphics::BatchRegion& lhs,
//                           const nc::graphics::BatchRegion& rhs) -> bool
// {
//     return lhs.meshId == rhs.meshId &&
//            lhs.passes == rhs.passes;
// }

TEST(InstanceCacheTest, Foo)
{
    auto uut = nc::graphics::InstanceCache2{};

    const auto id1 = 0;
    const auto id2 = 1;
    const auto id3 = 2;

    AddTestInstance(uut, id1, Batch1::objects.at(0));
    AddTestInstance(uut, id2, Batch1::objects.at(1));
    AddTestInstance(uut, id3, Batch2::objects.at(0));
    EXPECT_TRUE(uut.IsValidInstance(id1));
    EXPECT_TRUE(uut.IsValidInstance(id2));
    EXPECT_TRUE(uut.IsValidInstance(id3));
    VerifyBatch<Batch1>(uut, 0, 2, 2);
    VerifyBatch<Batch2>(uut, 2, 1, 1);

    {
        auto batches = uut.BuildBatches(g_allBatches);
        ASSERT_EQ(g_allBatches.size(), batches.size());
        EXPECT_EQ(0, batches.at(0).size());
        EXPECT_EQ(2, batches.at(1).size());
        EXPECT_EQ(0, batches.at(2).size());
    }

    uut.RemoveInstance(id1, Batch1::mesh.id, Batch1::passes);
    EXPECT_FALSE(uut.IsValidInstance(id1));
    VerifyBatch<Batch1>(uut, 0, 1, 2);
    VerifyBatch<Batch2>(uut, 2, 1, 1);

    uut.RemoveInstance(id2, Batch1::mesh.id, Batch1::passes);
    uut.RemoveInstance(id3, Batch2::mesh.id, Batch2::passes);
    EXPECT_FALSE(uut.IsValidInstance(id2));
    EXPECT_FALSE(uut.IsValidInstance(id3));
    VerifyBatch<Batch1>(uut, 0, 0, 2);
    VerifyBatch<Batch2>(uut, 2, 0, 1);
}

TEST(InstanceCacheTest, Foo2)
{
    auto uut = nc::graphics::InstanceCache2{};

    auto curId = 0;

    for (const auto& obj : Batch1::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    for (const auto& obj : Batch2::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    for (const auto& obj : Batch3::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    VerifyBatch<Batch1>(uut, 0, 3, 3);
    VerifyBatch<Batch2>(uut, 3, 3, 3);
    VerifyBatch<Batch3>(uut, 6, 3, 3);

    {
        auto batches = uut.BuildBatches(g_allBatches);
        ASSERT_EQ(g_allBatches.size(), batches.size());
        EXPECT_EQ(1, batches.at(0).size());
        EXPECT_EQ(3, batches.at(0).at(0).instanceCount);

        EXPECT_EQ(3, batches.at(1).size());
        EXPECT_EQ(3, batches.at(1).at(0).instanceCount);
        EXPECT_EQ(3, batches.at(1).at(1).instanceCount);
        EXPECT_EQ(3, batches.at(1).at(2).instanceCount);

        EXPECT_EQ(1, batches.at(2).size());
        EXPECT_EQ(3, batches.at(2).at(0).instanceCount);
    }

    for (const auto& obj : Batch1::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    for (const auto& obj : Batch2::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    for (const auto& obj : Batch3::objects)
    {
        AddTestInstance(uut, curId++, obj);
    }

    {
        auto batches = uut.BuildBatches(g_allBatches);
        ASSERT_EQ(g_allBatches.size(), batches.size());
        EXPECT_EQ(1, batches.at(0).size());
        EXPECT_EQ(3, batches.at(1).size());
        EXPECT_EQ(1, batches.at(2).size());
    }
}
