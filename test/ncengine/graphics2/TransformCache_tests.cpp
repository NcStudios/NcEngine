#include "gtest/gtest.h"
#include "graphics2/frontend/subsystem/TransformCache.h"
#include "ncengine/ecs/Ecs.h"
#include "../EcsFixture.inl"

auto MatrixEqual(DirectX::FXMMATRIX m1, DirectX::FXMMATRIX m2)
{
    return DirectX::XMVector4Equal(m1.r[0], m2.r[0]) &&
           DirectX::XMVector4Equal(m1.r[1], m2.r[1]) &&
           DirectX::XMVector4Equal(m1.r[2], m2.r[2]) &&
           DirectX::XMVector4Equal(m1.r[3], m2.r[3]);
};

TEST(TransformCacheTests, AddInstance)
{
    constexpr auto entity1 = nc::Entity{0, 0, 0};
    constexpr auto entity2 = nc::Entity{1, 0, 0};
    auto uut = nc::graphics::TransformCache{10};
    const auto first = uut.AddInstance(entity1);
    const auto second = uut.AddInstance(entity2);
    uut.CommitPendingChanges();

    EXPECT_EQ(first + 1, second);
    EXPECT_EQ(entity1, uut.GetEntity(first));
    EXPECT_EQ(entity2, uut.GetEntity(second));
}

TEST(TransformCacheTests, RemoveInstance)
{
    constexpr auto entity1 = nc::Entity{0, 0, 0};
    constexpr auto entity2 = nc::Entity{1, 0, 0};
    constexpr auto entity3 = nc::Entity{2, 0, 0};
    auto uut = nc::graphics::TransformCache{10};
    const auto first = uut.AddInstance(entity1);
    uut.AddInstance(entity2);
    uut.CommitPendingChanges();

    uut.RemoveInstance(first);
    uut.CommitPendingChanges();
    EXPECT_FALSE(uut.GetEntity(first).Valid());

    const auto recycledFirst = uut.AddInstance(entity3);
    uut.CommitPendingChanges();
    EXPECT_EQ(recycledFirst, first);
    EXPECT_EQ(entity3, uut.GetEntity(recycledFirst));
}

TEST(TransformCacheTests, UpdateMatrices)
{
    auto uut = nc::graphics::TransformCache{10};
    auto ecsFixture = EcsFixture{10};
    auto ecs = ecsFixture.GetTestWorld();

    const auto entity1 = ecs.Emplace<nc::Entity>(nc::EntityInfo{
        .position = nc::Vector3::Up()
    });

    const auto entity2 = ecs.Emplace<nc::Entity>(nc::EntityInfo{
        .position = nc::Vector3::Right(),
        .flags = nc::Entity::Flags::Static
    });

    const auto entity3 = ecs.Emplace<nc::Entity>(nc::EntityInfo{
        .position = nc::Vector3::Down()
    });

    const auto handle1 = uut.AddInstance(entity1);
    const auto handle2 = uut.AddInstance(entity2);
    const auto handle3 = uut.AddInstance(entity3);
    uut.CommitPendingChanges();
    uut.UpdateMatrices(ecs);

    // Matrices should match what is in the registry.
    const auto& expectedMatrix1 = ecs.Get<nc::Transform>(entity1).TransformationMatrix();
    const auto& expectedMatrix2 = ecs.Get<nc::Transform>(entity2).TransformationMatrix();
    const auto& expectedMatrix3 = ecs.Get<nc::Transform>(entity3).TransformationMatrix();
    const auto& actualMatrix1 = uut.GetInstance(handle1).modelMatrix;
    const auto& actualMatrix2 = uut.GetInstance(handle2).modelMatrix;
    const auto& actualMatrix3 = uut.GetInstance(handle3).modelMatrix;
    EXPECT_TRUE(MatrixEqual(expectedMatrix1, actualMatrix1));
    EXPECT_TRUE(MatrixEqual(expectedMatrix2, actualMatrix2));
    EXPECT_TRUE(MatrixEqual(expectedMatrix3, actualMatrix3));

    // Whole range, including the static entity, should be dirty on the first time through.
    auto updateInfo = uut.BuildState();
    EXPECT_EQ(3, updateInfo.instances.size());
    ASSERT_EQ(1, updateInfo.dirtyRanges.size());
    EXPECT_EQ(0, updateInfo.dirtyRanges[0].offset);
    EXPECT_EQ(3, updateInfo.dirtyRanges[0].count);

    // Subsequent calls should mark each dynamic object dirty. The static entity in the middle should split
    // the required update into two ranges.
    uut.UpdateMatrices(ecs);
    updateInfo = uut.BuildState();
    EXPECT_EQ(3, updateInfo.instances.size());
    ASSERT_EQ(2, updateInfo.dirtyRanges.size());
    EXPECT_EQ(0, updateInfo.dirtyRanges[0].offset);
    EXPECT_EQ(1, updateInfo.dirtyRanges[0].count);
    EXPECT_EQ(2, updateInfo.dirtyRanges[1].offset);
    EXPECT_EQ(1, updateInfo.dirtyRanges[1].count);

    // A removed transform should no longer get updated.
    ecs.Get<nc::Transform>(entity1).Translate(nc::Vector3::Front());
    uut.RemoveInstance(handle1);
    uut.CommitPendingChanges();
    uut.UpdateMatrices(ecs);
    updateInfo = uut.BuildState();
    EXPECT_EQ(3, updateInfo.instances.size());
    ASSERT_EQ(1, updateInfo.dirtyRanges.size());
    EXPECT_EQ(2, updateInfo.dirtyRanges[0].offset);
    EXPECT_EQ(1, updateInfo.dirtyRanges[0].count);
}

TEST(TransformCacheTests, MarkStaticsDirty)
{
    auto uut = nc::graphics::TransformCache{10};
    auto ecsFixture = EcsFixture{10};
    auto ecs = ecsFixture.GetTestWorld();

    const auto entity1 = ecs.Emplace<nc::Entity>(nc::EntityInfo{
        .position = nc::Vector3::Up(),
        .flags = nc::Entity::Flags::Static
    });

    const auto entity2 = ecs.Emplace<nc::Entity>(nc::EntityInfo{
        .position = nc::Vector3::Right(),
        .flags = nc::Entity::Flags::Static
    });

    const auto handle1 = uut.AddInstance(entity1);
    const auto handle2 = uut.AddInstance(entity2);
    uut.CommitPendingChanges();
    uut.UpdateMatrices(ecs);
    uut.BuildState(); // discard to clear dirty state

    // Double check statics aren't being reported as dirty.
    auto updateInfo = uut.BuildState();
    EXPECT_EQ(0, updateInfo.instances.size());
    EXPECT_EQ(0, updateInfo.dirtyRanges.size());

    // Move statics and notify.
    auto& transform1 = ecs.Get<nc::Transform>(entity1);
    auto& transform2 = ecs.Get<nc::Transform>(entity2);
    transform1.Translate(nc::Vector3::Front());
    transform2.Translate(nc::Vector3::Back());
    uut.MarkStaticsDirty();
    uut.UpdateMatrices(ecs);

    // Matrices should match what is in the registry.
    const auto& expectedMatrix1 = transform1.TransformationMatrix();
    const auto& expectedMatrix2 = transform2.TransformationMatrix();
    const auto& actualMatrix1 = uut.GetInstance(handle1).modelMatrix;
    const auto& actualMatrix2 = uut.GetInstance(handle2).modelMatrix;
    EXPECT_TRUE(MatrixEqual(expectedMatrix1, actualMatrix1));
    EXPECT_TRUE(MatrixEqual(expectedMatrix2, actualMatrix2));

    // Moved statics should included in update range.
    updateInfo = uut.BuildState();
    EXPECT_EQ(2, updateInfo.instances.size());
    EXPECT_EQ(1, updateInfo.dirtyRanges.size());
    EXPECT_EQ(0, updateInfo.dirtyRanges[0].offset);
    EXPECT_EQ(2, updateInfo.dirtyRanges[0].count);

    // Subsequent calls should ignore statics again.
    uut.UpdateMatrices(ecs);
    updateInfo = uut.BuildState();
    EXPECT_EQ(0, updateInfo.instances.size());
    EXPECT_EQ(0, updateInfo.dirtyRanges.size());
}
