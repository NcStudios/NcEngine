#include "gtest/gtest.h"
#include "physics/collision/narrow_phase/Simplex.h"

using namespace nc;
using namespace nc::physics;

constexpr auto v1 = Vector3::Splat(1.0f);
constexpr auto v2 = Vector3::Splat(2.0f);
constexpr auto v3 = Vector3::Splat(3.0f);
constexpr auto v4 = Vector3::Splat(4.0f);
constexpr auto v5 = Vector3::Splat(5.0f);

class Simplex_unit_tests : public ::testing::Test
{
    public:
        Simplex_unit_tests() : simplex{} {}
        ~Simplex_unit_tests() {}

        Simplex simplex;
};

TEST_F(Simplex_unit_tests, PushFront_LessThanCapacity_AddsNewValue)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    EXPECT_EQ(simplex[0], v1);
}

TEST_F(Simplex_unit_tests, PushFront_GreaterThanCapacity_DiscardsOldestValue)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.PushFront(v5, v5, v5, v5, v5);
    EXPECT_EQ(simplex[0], v5);
    EXPECT_EQ(simplex[1], v4);
    EXPECT_EQ(simplex[2], v3);
    EXPECT_EQ(simplex[3], v2);
}

TEST_F(Simplex_unit_tests, Size_AfterPushFront_UpdatesUntilCapacityReached)
{
    EXPECT_EQ(simplex.Size(), 0u);
    simplex.PushFront(v1, v2, v3, v4, v5);
    EXPECT_EQ(simplex.Size(), 1u);
    simplex.PushFront(v1, v2, v3, v4, v5);
    EXPECT_EQ(simplex.Size(), 2u);
    simplex.PushFront(v1, v2, v3, v4, v5);
    EXPECT_EQ(simplex.Size(), 3u);
    simplex.PushFront(v1, v2, v3, v4, v5);
    EXPECT_EQ(simplex.Size(), 4u);
    simplex.PushFront(v1, v2, v3, v4, v5);
    EXPECT_EQ(simplex.Size(), 4u);
}

TEST_F(Simplex_unit_tests, HasAllUniqueVertices_Empty_ReturnsTrue)
{
    EXPECT_TRUE(simplex.HasAllUniqueVertices());
}

TEST_F(Simplex_unit_tests, HasAllUniqueVertices_NoDuplicates_ReturnsTrue)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    EXPECT_TRUE(simplex.HasAllUniqueVertices());
}

TEST_F(Simplex_unit_tests, HasAllUniqueVertices_HasDuplicates_ReturnsFalse)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v1, v1, v1, v1, v1);
    EXPECT_FALSE(simplex.HasAllUniqueVertices());
}

TEST_F(Simplex_unit_tests, ToPoint_KeepPointA_DiscardsDownToPoint)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToPoint<0u>();
    ASSERT_EQ(simplex.Size(), 1u);
    EXPECT_EQ(simplex[0], v4);
}

TEST_F(Simplex_unit_tests, ToPoint_KeepPointB_DiscardsDownToPoint)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToPoint<1u>();
    ASSERT_EQ(simplex.Size(), 1u);
    EXPECT_EQ(simplex[0], v3);
}

TEST_F(Simplex_unit_tests, ToLine_KeepLineAB_DiscardsDownToLine)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToLine<0u, 1u>();
    ASSERT_EQ(simplex.Size(), 2u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v3);
}

TEST_F(Simplex_unit_tests, ToLine_KeepLineAC_DiscardsDownToLine)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToLine<0u, 2u>();
    ASSERT_EQ(simplex.Size(), 2u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v2);
}

TEST_F(Simplex_unit_tests, ToTriangle_KeepTriangleABC_DiscardsDownToTriangle)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToTriangle<0u, 1u, 2u>();
    ASSERT_EQ(simplex.Size(), 3u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v3);
    EXPECT_EQ(simplex[2], v2);
}

TEST_F(Simplex_unit_tests, ToTriangle_KeepTriangleACB_DiscardsDownToTriangle)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToTriangle<0u, 2u, 1u>();
    ASSERT_EQ(simplex.Size(), 3u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v2);
    EXPECT_EQ(simplex[2], v3);
}

TEST_F(Simplex_unit_tests, ToTriangle_KeepTriangleACD_DiscardsDownToTriangle)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToTriangle<0u, 2u, 3u>();
    ASSERT_EQ(simplex.Size(), 3u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v2);
    EXPECT_EQ(simplex[2], v1);
}

TEST_F(Simplex_unit_tests, ToTriangle_KeepTriangleADB_DiscardsDownToTriangle)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToTriangle<0u, 3u, 1u>();
    ASSERT_EQ(simplex.Size(), 3u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v1);
    EXPECT_EQ(simplex[2], v3);
}

TEST_F(Simplex_unit_tests, ToTriangle_KeepTriangleABD_DiscardsDownToTriangle)
{
    simplex.PushFront(v1, v1, v1, v1, v1);
    simplex.PushFront(v2, v2, v2, v2, v2);
    simplex.PushFront(v3, v3, v3, v3, v3);
    simplex.PushFront(v4, v4, v4, v4, v4);
    simplex.ToTriangle<0u, 1u, 3u>();
    ASSERT_EQ(simplex.Size(), 3u);
    EXPECT_EQ(simplex[0], v4);
    EXPECT_EQ(simplex[1], v3);
    EXPECT_EQ(simplex[2], v1);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}