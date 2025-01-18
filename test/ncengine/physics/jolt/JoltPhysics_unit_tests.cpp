#include "gtest/gtest.h"
#include "JobSystem_stub.inl"
#include "physics/jolt/JoltPhysics.h"
#include "ncengine/config/Config.h"
#include "ncengine/physics/PhysicsSnapshot.h"

constexpr auto g_memorySettings = nc::config::MemorySettings{};
constexpr auto g_physicsSettings = nc::config::PhysicsSettings{
    .tempAllocatorSize = 1024 * 1024 * 4,
    .maxBodyPairs = 8,
    .maxContacts = 4
};

TEST(JoltPhysicsIntegrationTest, Initialize_succeeds)
{
    EXPECT_NO_THROW(nc::physics::JoltPhysics(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{}));
}

TEST(JoltPhysicsIntegrationTest, Tick_trivialCall_succeeds)
{
    auto uut = nc::physics::JoltPhysics(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{});
    EXPECT_NO_THROW(uut.Tick(1.0f / 60.0f));
}

TEST(JoltPhysicsIntegrationTest, SaveRestoreSnapshot_succeeds)
{
    auto uut = nc::physics::JoltPhysics(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{});
    const auto initialTick = uut.currentTick;

    auto snapshot = nc::PhysicsSnapshot{};
    EXPECT_NO_THROW(uut.SaveSnapshot(snapshot));

    EXPECT_TRUE(snapshot.IsValid());
    EXPECT_NE(0, snapshot.GetSize());
    EXPECT_EQ(initialTick, snapshot.GetTick());
    EXPECT_EQ(initialTick, uut.currentTick);

    uut.Tick(1.0f / 60.0f);
    EXPECT_EQ(initialTick + 1, uut.currentTick);

    ASSERT_TRUE(uut.RestoreFromSnapshot(snapshot));
    EXPECT_EQ(initialTick, uut.currentTick);

    uut.Tick(1.0f / 60.0f);
    snapshot.ResetRead();
    ASSERT_TRUE(uut.RestoreFromSnapshot(snapshot));
    EXPECT_EQ(initialTick, uut.currentTick);
}

TEST(JoltPhysicsIntegrationTest, SaveRestoreSnapshot_failureCases)
{
    auto uut = nc::physics::JoltPhysics(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{});

    auto snapshotT1 = nc::PhysicsSnapshot{};
    auto snapshotT2 = nc::PhysicsSnapshot{};
    uut.SaveSnapshot(snapshotT1);
    uut.Tick(1.0f / 60.0f);
    uut.SaveSnapshot(snapshotT2);

    uut.RestoreFromSnapshot(snapshotT1);
    EXPECT_THROW(uut.RestoreFromSnapshot(snapshotT2), nc::NcError); // snapshot newer than simulation tick

    uut.Tick(1.0f / 60.0f);
    snapshotT1.Clear();
    EXPECT_THROW(uut.RestoreFromSnapshot(snapshotT1), nc::NcError); // invalid snapshot
}
