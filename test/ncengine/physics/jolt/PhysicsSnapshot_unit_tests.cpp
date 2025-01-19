#include "JoltApiFixture.inl"
#include "ncengine/physics/PhysicsSnapshot.h"
#include "ncutility/ScopeExit.h"

#include <ranges>

class PhysicsSnapshotTests : public JoltApiFixture
{
    protected:

        // nc::Signal<const nc::asset::ConvexHullUpdateEventData&> convexHullSignal;
        // nc::Signal<const nc::asset::MeshColliderUpdateEventData&> meshColliderSignal;
        // nc::physics::ShapeFactory shapeFactory;
        // nc::physics::BodyFactory uut;

        // BodyFactoryTest()
        //     : shapeFactory{convexHullSignal, meshColliderSignal},
        //       uut{joltApi.physicsSystem.GetBodyInterfaceNoLock(), shapeFactory}
        // {
        // }

        // void DestroyBody(const JPH::Body* body)
        // {
        //     auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
        //     const auto id = body->GetID();
        //     interface.DestroyBody(id); // note: factory doesn't add body, so don't call remove
        // }
};

TEST_F(PhysicsSnapshotTests, Constructor_default_createsEmptySnapshot)
{
    const auto uut = nc::PhysicsSnapshot{};
    EXPECT_FALSE(uut.IsValid());
    EXPECT_EQ(nc::PhysicsTick::NullTick, uut.GetTick().Value());
    EXPECT_EQ(0, uut.GetSize());
}

TEST_F(PhysicsSnapshotTests, Constructor_fromBytes_createsNonEmptySnapshot)
{
    const auto expectedTick = nc::PhysicsTick{42};
    const auto expectedBytes = std::vector<uint8_t>(8);
    const auto uut = nc::PhysicsSnapshot{expectedTick, expectedBytes};

    EXPECT_TRUE(uut.IsValid());
    EXPECT_EQ(expectedTick, uut.GetTick());
    ASSERT_EQ(expectedBytes.size(), uut.GetSize());
    EXPECT_TRUE(std::ranges::equal(expectedBytes, uut.ViewBuffer()));
}

TEST_F(PhysicsSnapshotTests, ExtractBuffer_clearsSnapshot)
{
    const auto tick = nc::PhysicsTick{42};
    const auto expectedBytes = std::vector<uint8_t>(8);
    auto uut = nc::PhysicsSnapshot{tick, expectedBytes};

    const auto actualBytes = uut.ExtractBuffer();
    EXPECT_FALSE(uut.IsValid());
    EXPECT_EQ(nc::PhysicsTick::NullTick, uut.GetTick().Value());
    EXPECT_EQ(0, uut.GetSize());
    ASSERT_EQ(expectedBytes.size(), actualBytes.size());
    EXPECT_TRUE(std::ranges::equal(expectedBytes, actualBytes));
}

TEST_F(PhysicsSnapshotTests, Clear_empty_triviallySucceeds)
{
    auto uut = nc::PhysicsSnapshot{};
    EXPECT_NO_THROW(uut.Clear());
    EXPECT_FALSE(uut.IsValid());
    EXPECT_EQ(nc::PhysicsTick::NullTick, uut.GetTick().Value());
    EXPECT_EQ(0, uut.GetSize());
}

TEST_F(PhysicsSnapshotTests, Clear_nonempty_resetsSnapshot)
{
    const auto tick = nc::PhysicsTick{42};
    const auto expectedBytes = std::vector<uint8_t>(8);
    auto uut = nc::PhysicsSnapshot{tick, expectedBytes};

    EXPECT_NO_THROW(uut.Clear());
    EXPECT_FALSE(uut.IsValid());
    EXPECT_EQ(nc::PhysicsTick::NullTick, uut.GetTick().Value());
    EXPECT_EQ(0, uut.GetSize());
}

TEST_F(PhysicsSnapshotTests, SaveRestore_sampleWorkflow)
{
    const auto body1 = CreateBody();
    const auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    constexpr auto dt = 1.0f / 60.0f;
    auto uutT0 = nc::PhysicsSnapshot{};
    auto uutT1 = nc::PhysicsSnapshot{};

    // Save tick 0 and 1
    joltApi.SaveSnapshot(uutT0);
    EXPECT_TRUE(uutT0.IsValid());
    EXPECT_EQ(joltApi.currentTick, uutT0.GetTick());
    joltApi.Tick(dt);
    joltApi.SaveSnapshot(uutT1);
    EXPECT_TRUE(uutT1.IsValid());
    EXPECT_EQ(joltApi.currentTick, uutT1.GetTick());

    // Restore to tick 0
    ASSERT_TRUE(joltApi.RestoreFromSnapshot(uutT0));
    EXPECT_EQ(joltApi.currentTick, uutT0.GetTick());
    EXPECT_EQ(0, uutT0.GetTick().Value());

    // Can re-restore to same snapshot
    joltApi.Tick(dt);
    uutT0.ResetRead();
    ASSERT_TRUE(joltApi.RestoreFromSnapshot(uutT0));
    EXPECT_EQ(joltApi.currentTick, uutT0.GetTick());
    EXPECT_EQ(0, uutT0.GetTick().Value());

    // Can overwrite snapshot
    joltApi.SaveSnapshot(uutT0);
    joltApi.Tick(dt);
    ASSERT_TRUE(joltApi.RestoreFromSnapshot(uutT0));
    EXPECT_EQ(joltApi.currentTick, uutT0.GetTick());
    EXPECT_EQ(0, uutT0.GetTick().Value());

    // Cannot restore into the future
    ASSERT_GT(uutT1.GetTick(), joltApi.currentTick);
    EXPECT_THROW(joltApi.RestoreFromSnapshot(uutT1), nc::NcError);

    // Restore succeeds once we're passed a snapshot
    joltApi.Tick(dt, 2);
    ASSERT_LT(uutT1.GetTick(), joltApi.currentTick);
    ASSERT_TRUE(joltApi.RestoreFromSnapshot(uutT1));
    EXPECT_EQ(joltApi.currentTick, uutT1.GetTick());

    // Cleared snapshot fails
    uutT0.Clear();
    EXPECT_THROW(joltApi.RestoreFromSnapshot(uutT0), nc::NcError);
}
