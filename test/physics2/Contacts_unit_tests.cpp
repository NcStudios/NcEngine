#include "gtest/gtest.h"
#include "ncengine/physics/HitInfo.h"

#include <algorithm>

TEST(ContactsTest, InitializeWithMaxSize_hasExpectedPoints)
{
    constexpr auto expectedPoints = std::array{
        nc::Vector3{1.0f, 2.0f, 3.0f},
        nc::Vector3{4.0f, 5.0f, 6.0f},
        nc::Vector3{7.0f, 8.0f, 9.0f},
        nc::Vector3{10.0f, 11.0f, 12.0f}
    };

    const auto uut = nc::physics::Contacts{expectedPoints, 4};
    const auto actualPoints = uut.GetPoints();
    EXPECT_EQ(4, actualPoints.size());
    EXPECT_EQ(4, uut.GetCount());
    EXPECT_TRUE(std::ranges::equal(expectedPoints, actualPoints));
}

TEST(ContactsTest, InitializeWithPartialSize_hasExpectedPoints)
{
    constexpr auto expectedPoints = std::array{
        nc::Vector3{1.0f, 2.0f, 3.0f},
        nc::Vector3{4.0f, 5.0f, 6.0f},
        nc::Vector3{0.0f, 0.0f, 0.0f},
        nc::Vector3{0.0f, 0.0f, 0.0f}
    };

    const auto uut = nc::physics::Contacts{expectedPoints, 2};
    const auto actualPoints = uut.GetPoints();
    EXPECT_EQ(2, actualPoints.size());
    EXPECT_EQ(2, uut.GetCount());

    EXPECT_TRUE(std::ranges::equal(
        expectedPoints.begin(),
        expectedPoints.begin() + 2,
        actualPoints.begin(),
        actualPoints.end()
    ));
}

TEST(ContactsTest, InitializeWithPartialSize_hasNoPoints)
{
    constexpr auto expectedPoints = std::array<nc::Vector3, 4>{};
    const auto uut = nc::physics::Contacts{expectedPoints, 0};
    const auto actualPoints = uut.GetPoints();
    EXPECT_EQ(0, actualPoints.size());
    EXPECT_EQ(0, uut.GetCount());
}
