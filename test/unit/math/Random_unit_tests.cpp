#include "gtest/gtest.h"
#include "math/Random.h"

using namespace nc;

TEST(Random_unit_tests, Fork_NewInstanceSeededFromNextValue)
{
    constexpr size_t seed = 12345;
    Random original{seed};
    Random copy = original;
    Random forked = original.Fork();
    auto expectedSeed = copy.GetU64();
    auto actualSeed = forked.Seed();
    EXPECT_EQ(expectedSeed, actualSeed);
}

TEST(Random_unit_tests, Seed_GetsAndSetsSeedValue)
{
    Random random{128};
    EXPECT_EQ(random.Seed(), 128);
    random.Seed(2);
    EXPECT_EQ(random.Seed(), 2);
}

TEST(Random_unit_tests, Get_ReturnsValueWithinExpectedRange)
{
    Random random{1};

    auto f = random.Get();
    EXPECT_GE(f, 0.0f);
    EXPECT_LE(f, 1.0f);

    auto v2 = random.GetVector2();
    EXPECT_GE(v2.x, 0.0f);
    EXPECT_LE(v2.x, 1.0f);
    EXPECT_GE(v2.y, 0.0f);
    EXPECT_LE(v2.y, 1.0f);

    auto v3 = random.GetVector3();
    EXPECT_GE(v3.x, 0.0f);
    EXPECT_LE(v3.x, 1.0f);
    EXPECT_GE(v3.y, 0.0f);
    EXPECT_LE(v3.y, 1.0f);
    EXPECT_GE(v3.z, 0.0f);
    EXPECT_LE(v3.z, 1.0f);

    auto v4 = random.GetVector4();
    EXPECT_GE(v4.x, 0.0f);
    EXPECT_LE(v4.x, 1.0f);
    EXPECT_GE(v4.y, 0.0f);
    EXPECT_LE(v4.y, 1.0f);
    EXPECT_GE(v4.z, 0.0f);
    EXPECT_LE(v4.z, 1.0f);
    EXPECT_GE(v4.w, 0.0f);
    EXPECT_LE(v4.w, 1.0f);
}

TEST(Random_unit_tests, Between_ReturnsValueWithinExpectedRange)
{
    constexpr float min = -100.0f;
    constexpr float max = 28.0f;
    Random random{1};

    auto f = random.Between(min, max);
    EXPECT_GE(f, min);
    EXPECT_LE(f, max);

    auto v2 = random.Between(Vector2::Splat(min), Vector2::Splat(max));
    EXPECT_GE(v2.x, min);
    EXPECT_LE(v2.x, max);
    EXPECT_GE(v2.y, min);
    EXPECT_LE(v2.y, max);

    auto v3 = random.Between(Vector3::Splat(min), Vector3::Splat(max));
    EXPECT_GE(v3.x, min);
    EXPECT_LE(v3.x, max);
    EXPECT_GE(v3.y, min);
    EXPECT_LE(v3.y, max);
    EXPECT_GE(v3.z, min);
    EXPECT_LE(v3.z, max);

    auto v4 = random.Between(Vector4::Splat(min), Vector4::Splat(max));
    EXPECT_GE(v4.x, min);
    EXPECT_LE(v4.x, max);
    EXPECT_GE(v4.y, min);
    EXPECT_LE(v4.y, max);
    EXPECT_GE(v4.z, min);
    EXPECT_LE(v4.z, max);
    EXPECT_GE(v4.w, min);
    EXPECT_LE(v4.w, max);
}

TEST(Random_unit_tests, Between_ZeroRange_ReturnsValueEqualToRangeOffset)
{
    Random random{1};

    auto f = random.Between(15.0f, 15.0f);
    EXPECT_FLOAT_EQ(f, 15.0f);

    auto v2 = random.Between(Vector2::Splat(15.0f), Vector2::Splat(15.0f));
    EXPECT_FLOAT_EQ(v2.x, 15.0f);
    EXPECT_FLOAT_EQ(v2.y, 15.0f);

    auto v3 = random.Between(Vector3::Splat(15.0f), Vector3::Splat(15.0f));
    EXPECT_FLOAT_EQ(v3.x, 15.0f);
    EXPECT_FLOAT_EQ(v3.y, 15.0f);
    EXPECT_FLOAT_EQ(v3.z, 15.0f);

    auto v4 = random.Between(Vector4::Splat(15.0f), Vector4::Splat(15.0f));
    EXPECT_FLOAT_EQ(v4.x, 15.0f);
    EXPECT_FLOAT_EQ(v4.y, 15.0f);
    EXPECT_FLOAT_EQ(v4.z, 15.0f);
    EXPECT_FLOAT_EQ(v4.w, 15.0f);
}

TEST(Random_unit_tests, EqualityOperator_EqualStates_ReturnsTrue)
{
    Random r1{5};
    Random r2{5};
    EXPECT_EQ(r1, r2);
    r1.Get();
    r1.Get();
    r1.Get();
    r2.Get();
    r2.Get();
    r2.Get();
    EXPECT_EQ(r1, r2);
}

TEST(Random_unit_tests, InqualityOperator_DifferentStates_ReturnsTrue)
{
    Random r1{5};
    Random r2{1};
    EXPECT_NE(r1, r2);

    Random r3 = r1;
    EXPECT_EQ(r1, r3);
    r3.Get();
    EXPECT_NE(r1, r3);
}

TEST(Random_unit_tests, Copy_CreatedWithSameState)
{
    Random original{5};
    Random copy = original;
    EXPECT_EQ(original, copy);

    auto vOriginal = original.Get();
    auto vCopy = copy.Get();
    EXPECT_EQ(vOriginal, vCopy);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}