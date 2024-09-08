#include "gtest/gtest.h"
#include "physics2/jolt/JoltApi.h"

TEST(JoltApiIntegrationTest, Initialize_succeeds)
{
    EXPECT_NO_THROW(nc::physics::JoltApi::Initialize());
}

TEST(JoltApiIntegrationTest, Update_trivialCall_succeeds)
{
    auto uut = nc::physics::JoltApi::Initialize();
    EXPECT_NO_THROW(uut.Update(1.0f / 60.0f));
}
