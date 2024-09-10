#include "physics2/jolt/JoltApi.h"
#include "gtest/gtest.h"

namespace nc::task
{
class AsyncDispatcher{};
}

TEST(JoltApiIntegrationTest, Initialize_succeeds)
{
    EXPECT_NO_THROW(nc::physics::JoltApi::Initialize(nc::task::AsyncDispatcher{}));
}

TEST(JoltApiIntegrationTest, Update_trivialCall_succeeds)
{
    auto uut = nc::physics::JoltApi::Initialize(nc::task::AsyncDispatcher{});
    EXPECT_NO_THROW(uut.Update(1.0f / 60.0f));
}
