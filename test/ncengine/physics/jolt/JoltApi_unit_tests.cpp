#include "gtest/gtest.h"
#include "JobSystem_stub.inl"
#include "physics/jolt/JoltApi.h"
#include "ncengine/config/Config.h"

constexpr auto g_memorySettings = nc::config::MemorySettings{};
constexpr auto g_physicsSettings = nc::config::PhysicsSettings{
    .tempAllocatorSize = 1024 * 1024 * 4,
    .maxBodyPairs = 8,
    .maxContacts = 4
};

TEST(JoltApiIntegrationTest, Initialize_succeeds)
{
    EXPECT_NO_THROW(nc::physics::JoltApi::Initialize(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{}));
}

TEST(JoltApiIntegrationTest, Update_trivialCall_succeeds)
{
    auto uut = nc::physics::JoltApi::Initialize(g_memorySettings, g_physicsSettings, nc::task::AsyncDispatcher{});
    EXPECT_NO_THROW(uut.Update(1.0f / 60.0f));
}
