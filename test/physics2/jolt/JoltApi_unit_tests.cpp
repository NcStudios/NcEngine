#include "physics2/jolt/JoltApi.h"
#include "gtest/gtest.h"
#include "ncengine/config/Config.h"

constexpr auto g_memorySettings = nc::config::MemorySettings{};
constexpr auto g_physicsSettings = nc::config::PhysicsSettings{
    .tempAllocatorSize = 1024 * 1024 * 4,
    .maxBodyPairs = 8,
    .maxContacts = 4
};

TEST(JoltApiIntegrationTest, Initialize_succeeds)
{
    EXPECT_NO_THROW(nc::physics::JoltApi::Initialize(g_memorySettings, g_physicsSettings));
}

TEST(JoltApiIntegrationTest, Update_trivialCall_succeeds)
{
    auto uut = nc::physics::JoltApi::Initialize(g_memorySettings, g_physicsSettings);
    EXPECT_NO_THROW(uut.Update(1.0f / 60.0f));
}
