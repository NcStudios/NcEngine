#include "NcaHeader.h"
#include "gtest/gtest.h"

TEST(VersionTests, IsVersionSupported_version4_returnsTrue)
{
    EXPECT_TRUE(nc::asset::IsVersionSupported(nc::asset::version4));
}

TEST(VersionTests, IsVersionSupported_currentVersion_returnsTrue)
{
    EXPECT_TRUE(nc::asset::IsVersionSupported(nc::asset::currentVersion));
}

TEST(VersionTests, IsVersionSupported_badVersion_returnsFalse)
{
    EXPECT_FALSE(nc::asset::IsVersionSupported(1ull));
}
