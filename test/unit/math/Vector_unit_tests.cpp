#include "gtest/gtest.h"
#include "math/Vector.h"

using namespace nc;

TEST(Vector_unit_tests, Vector2_Initialization)
{
    auto defaultVec = Vector2{};
    EXPECT_FLOAT_EQ(defaultVec.x, 0.0f);
    EXPECT_FLOAT_EQ(defaultVec.y, 0.0f);

    auto splat = Vector2::Splat(2.0f);
    EXPECT_FLOAT_EQ(splat.x, 2.0f);
    EXPECT_FLOAT_EQ(splat.y, 2.0f);

    auto one = Vector2::One();
    EXPECT_FLOAT_EQ(one.x, 1.0f);
    EXPECT_FLOAT_EQ(one.y, 1.0f);

    auto zero = Vector2::Zero();
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);

    auto up = Vector2::Up();
    EXPECT_FLOAT_EQ(up.x, 0.0f);
    EXPECT_FLOAT_EQ(up.y, 1.0f);

    auto down = Vector2::Down();
    EXPECT_FLOAT_EQ(down.x, 0.0f);
    EXPECT_FLOAT_EQ(down.y, -1.0f);

    auto left = Vector2::Left();
    EXPECT_FLOAT_EQ(left.x, -1.0f);
    EXPECT_FLOAT_EQ(left.y, 0.0f);

    auto right = Vector2::Right();
    EXPECT_FLOAT_EQ(right.x, 1.0f);
    EXPECT_FLOAT_EQ(right.y, 0.0f);
}

TEST(Vector_unit_tests, Vector2_OperatorEquals)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{1.0f, 2.0f};
    EXPECT_TRUE(a == b);

    Vector2 c{3.0f, 3.0f};
    EXPECT_FALSE(a == c);
}

TEST(Vector_unit_tests, Vector2_OperatorNotEqual)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{1.0f, 2.0f};
    EXPECT_FALSE(a != b);

    Vector2 c{2.0f, 2.0f};
    EXPECT_TRUE(a != c);
}

TEST(Vector_unit_tests, Vector2_OperatorPlus)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{2.0f, 0.5f};
    auto actual = a + b;
    EXPECT_EQ(actual, Vector2(3.0f, 2.5f));
}

TEST(Vector_unit_tests, Vector2_OperatorMinus)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{2.0f, 0.5f};
    auto actual = a - b;
    EXPECT_EQ(actual, Vector2(-1.0f, 1.5f));
}

TEST(Vector_unit_tests, Vector2_OperatorMultiply)
{
    Vector2 v{1.0f, 2.0f};
    float s = 2.0f;
    auto actual = v * s;
    EXPECT_EQ(actual, Vector2(2.0f, 4.0f));
    EXPECT_EQ(actual, s * v);
}

TEST(Vector_unit_tests, Vector2_OperatorDivide)
{
    Vector2 v{1.0f, 2.0f};
    float s = 2.0f;
    auto actual = v / s;
    EXPECT_EQ(actual, Vector2(0.5f, 1.0f));
}

TEST(Vector_unit_tests, Vector2_OperatorNegate)
{
    Vector2 v{1.0f, 2.0f};
    auto actual = -v;
    EXPECT_EQ(actual, Vector2(-1.0f, -2.0f));
}

TEST(Vector_unit_tests, Vector2_Dot)
{
    Vector2 a{2.0f, 3.0f};
    Vector2 b{3.0f, 4.0f};
    auto actual = Dot(a, b);
    EXPECT_FLOAT_EQ(actual, 18.0f);
}

TEST(Vector_unit_tests, Vector2_HadamardProduct)
{
    Vector2 a{2.0f, 3.0f};
    Vector2 b{3.0f, 4.0f};
    auto actual = HadamardProduct(a, b);
    EXPECT_EQ(actual, Vector2(6.0f, 12.0f));
}

TEST(Vector_unit_tests, Vector2_SquareMagnitude)
{
    Vector2 a{2.0f, 2.0f};
    auto actual = SquareMagnitude(a);
    EXPECT_FLOAT_EQ(actual, 8.0f);
}

TEST(Vector_unit_tests, Vector2_Magnitude)
{
    Vector2 a{2.0f, 2.0f};
    auto actual = Magnitude(a);
    EXPECT_FLOAT_EQ(actual, std::sqrt(8.0f));
}

TEST(Vector_unit_tests, Vector2_Normalize)
{
    Vector2 a{2.0f, 2.0f};
    auto actual = Normalize(a);
    EXPECT_FLOAT_EQ(actual.x, 2.0f / std::sqrt(8.0f));
    EXPECT_FLOAT_EQ(actual.x, actual.y);
}

TEST(Vector_unit_tests, Vector2_Lerp)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{2.0f, 3.0f};
    float factor = 0.5f;
    auto actual = Lerp(a, b, factor);
    EXPECT_FLOAT_EQ(actual.x, 1.5f);
    EXPECT_FLOAT_EQ(actual.y, 2.5f);
}

TEST(Vector_unit_tests, Vector2_SquareDistance)
{
    Vector2 a{1.0f, 1.0f};
    Vector2 b{2.0f, 2.0f};
    auto actual = SquareDistance(a, b);
    EXPECT_FLOAT_EQ(actual, 2.0f);
}

TEST(Vector_unit_tests, Vector2_Distance)
{
    Vector2 a{1.0f, 1.0f};
    Vector2 b{2.0f, 2.0f};
    auto actual = Distance(a, b);
    EXPECT_FLOAT_EQ(actual, std::sqrt(2));
}

TEST(Vector_unit_tests, Vector2_HasAnyZeroElement)
{
    Vector2 a{1.0f, 2.0f};
    Vector2 b{0.0f, 1.0f};
    Vector2 c{1.0f, 0.0f};
    EXPECT_FALSE(HasAnyZeroElement(a));
    EXPECT_TRUE(HasAnyZeroElement(b));
    EXPECT_TRUE(HasAnyZeroElement(c));
}

////////////////////////////////////

TEST(Vector_unit_tests, Vector3_Initialization)
{
    auto defaultVec = Vector3{};
    EXPECT_EQ(defaultVec, Vector3(0,0,0));

    auto splat = Vector3::Splat(2.0f);
    EXPECT_EQ(splat, Vector3(2,2,2));

    auto one = Vector3::One();
    EXPECT_EQ(one, Vector3(1,1,1));

    auto zero = Vector3::Zero();
    EXPECT_EQ(zero, Vector3(0,0,0));

    auto up = Vector3::Up();
    EXPECT_EQ(up, Vector3(0,1,0));

    auto down = Vector3::Down();
    EXPECT_EQ(down, Vector3(0,-1,0));

    auto left = Vector3::Left();
    EXPECT_EQ(left, Vector3(-1,0,0));

    auto right = Vector3::Right();
    EXPECT_EQ(right, Vector3(1,0,0));

    auto front = Vector3::Front();
    EXPECT_EQ(front, Vector3(0,0,1));

    auto back = Vector3::Back();
    EXPECT_EQ(back, Vector3(0,0,-1));
}

TEST(Vector_unit_tests, Vector3_OperatorEquals)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{1.0f, 2.0f, 3.0f};
    EXPECT_TRUE(a == b);

    Vector3 c{3.0f, 3.0f, 3.0f};
    EXPECT_FALSE(a == c);
}

TEST(Vector_unit_tests, Vector3_OperatorNotEqual)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{1.0f, 2.0f, 3.0f};
    EXPECT_FALSE(a != b);

    Vector3 c{2.0f, 2.0f, 2.0f};
    EXPECT_TRUE(a != c);
}

TEST(Vector_unit_tests, Vector3_OperatorPlus)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{2.0f, 0.5f, 0.0f};
    auto actual = a + b;
    EXPECT_EQ(actual, Vector3(3.0f, 2.5f, 3.0f));
}

TEST(Vector_unit_tests, Vector3_OperatorMinus)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{2.0f, 0.5f, 0.0f};
    auto actual = a - b;
    EXPECT_EQ(actual, Vector3(-1.0f, 1.5f, 3.0f));
}

TEST(Vector_unit_tests, Vector3_OperatorMultiply)
{
    Vector3 v{1.0f, 2.0f, 3.0f};
    float s = 2.0f;
    auto actual = v * s;
    EXPECT_EQ(actual, Vector3(2.0f, 4.0f, 6.0f));
    EXPECT_EQ(actual, s * v);
}

TEST(Vector_unit_tests, Vector3_OperatorDivide)
{
    Vector3 v{1.0f, 2.0f, 3.0f};
    float s = 2.0f;
    auto actual = v / s;
    EXPECT_EQ(actual, Vector3(0.5f, 1.0f, 1.5f));
}

TEST(Vector_unit_tests, Vector3_OperatorNegate)
{
    Vector3 v{1.0f, 2.0f, 3.0f};
    auto actual = -v;
    EXPECT_EQ(actual, Vector3(-1.0f, -2.0f, -3.0f));
}

TEST(Vector_unit_tests, Vector3_Dot)
{
    Vector3 a{2.0f, 3.0f, 4.0f};
    Vector3 b{3.0f, 4.0f, 5.0f};
    auto actual = Dot(a, b);
    EXPECT_FLOAT_EQ(actual, 38.0f);
}

TEST(Vector_unit_tests, Vector3_HadamardProduct)
{
    Vector3 a{2.0f, 3.0f, 4.0f};
    Vector3 b{3.0f, 4.0f, 5.0f};
    auto actual = HadamardProduct(a, b);
    EXPECT_EQ(actual, Vector3(6.0f, 12.0f, 20.0f));
}

TEST(Vector_unit_tests, Vector3_SquareMagnitude)
{
    Vector3 a{2.0f, 2.0f, 2.0f};
    auto actual = SquareMagnitude(a);
    EXPECT_FLOAT_EQ(actual, 12.0f);
}

TEST(Vector_unit_tests, Vector3_Magnitude)
{
    Vector3 a{2.0f, 2.0f, 2.0f};
    auto actual = Magnitude(a);
    EXPECT_FLOAT_EQ(actual, std::sqrt(12.0f));
}

TEST(Vector_unit_tests, Vector3_Normalize)
{
    Vector3 a{2.0f, 2.0f, 2.0f};
    auto actual = Normalize(a);
    EXPECT_FLOAT_EQ(actual.x, 2.0f / std::sqrt(12.0f));
    EXPECT_FLOAT_EQ(actual.x, actual.y);
    EXPECT_FLOAT_EQ(actual.y, actual.z);
}

TEST(Vector_unit_tests, Vector3_Lerp)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{2.0f, 3.0f, 4.0f};
    float factor = 0.5f;
    auto actual = Lerp(a, b, factor);
    EXPECT_FLOAT_EQ(actual.x, 1.5f);
    EXPECT_FLOAT_EQ(actual.y, 2.5f);
    EXPECT_FLOAT_EQ(actual.z, 3.5f);
}

TEST(Vector_unit_tests, Vector3_SquareDistance)
{
    Vector3 a{1.0f, 1.0f, 1.0f};
    Vector3 b{2.0f, 2.0f, 2.0f};
    auto actual = SquareDistance(a, b);
    EXPECT_FLOAT_EQ(actual, 3.0f);
}

TEST(Vector_unit_tests, Vector3_Distance)
{
    Vector3 a{1.0f, 1.0f, 1.0f};
    Vector3 b{2.0f, 2.0f, 2.0f};
    auto actual = Distance(a, b);
    EXPECT_FLOAT_EQ(actual, std::sqrt(3));
}

TEST(Vector_unit_tests, Vector3_HasAnyZeroElement)
{
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{0.0f, 1.0f, 1.0f};
    Vector3 c{1.0f, 0.0f, 0.0f};
    EXPECT_FALSE(HasAnyZeroElement(a));
    EXPECT_TRUE(HasAnyZeroElement(b));
    EXPECT_TRUE(HasAnyZeroElement(c));
}

/////////

TEST(Vector_unit_tests, Vector4_Initialization)
{
    auto defaultVec = Vector4{};
    EXPECT_EQ(defaultVec, Vector4(0,0,0,0));

    auto splat = Vector4::Splat(2.0f);
    EXPECT_EQ(splat, Vector4(2,2,2,2));

    auto one = Vector4::One();
    EXPECT_EQ(one, Vector4(1,1,1,1));

    auto zero = Vector4::Zero();
    EXPECT_EQ(zero, Vector4(0,0,0,0));
}

TEST(Vector_unit_tests, Vector4_OperatorEquals)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{1.0f, 2.0f, 3.0f, 4.0f};
    EXPECT_TRUE(a == b);

    Vector4 c{3.0f, 3.0f, 3.0f, 3.0f};
    EXPECT_FALSE(a == c);
}

TEST(Vector_unit_tests, Vector4_OperatorNotEqual)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{1.0f, 2.0f, 3.0f, 4.0f};
    EXPECT_FALSE(a != b);

    Vector4 c{2.0f, 2.0f, 2.0f, 2.0f};
    EXPECT_TRUE(a != c);
}

TEST(Vector_unit_tests, Vector4_OperatorPlus)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{2.0f, 0.5f, 0.0f, 1.0f};
    auto actual = a + b;
    EXPECT_EQ(actual, Vector4(3.0f, 2.5f, 3.0f, 5.0f));
}

TEST(Vector_unit_tests, Vector4_OperatorMinus)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{2.0f, 0.5f, 0.0f, 1.0f};
    auto actual = a - b;
    EXPECT_EQ(actual, Vector4(-1.0f, 1.5f, 3.0f, 3.0f));
}

TEST(Vector_unit_tests, Vector4_OperatorMultiply)
{
    Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
    float s = 2.0f;
    auto actual = v * s;
    EXPECT_EQ(actual, Vector4(2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_EQ(actual, s * v);
}

TEST(Vector_unit_tests, Vector4_OperatorDivide)
{
    Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
    float s = 2.0f;
    auto actual = v / s;
    EXPECT_EQ(actual, Vector4(0.5f, 1.0f, 1.5f, 2.0f));
}

TEST(Vector_unit_tests, Vector4_OperatorNegate)
{
    Vector4 v{1.0f, 2.0f, 3.0f, 4.0f};
    auto actual = -v;
    EXPECT_EQ(actual, Vector4(-1.0f, -2.0f, -3.0f, -4.0f));
}

TEST(Vector_unit_tests, Vector4_Dot)
{
    Vector4 a{2.0f, 3.0f, 4.0f, 1.0f};
    Vector4 b{3.0f, 4.0f, 5.0f, 1.0f};
    auto actual = Dot(a, b);
    EXPECT_FLOAT_EQ(actual, 39.0f);
}

TEST(Vector_unit_tests, Vector4_HadamardProduct)
{
    Vector4 a{2.0f, 3.0f, 4.0f, 1.0f};
    Vector4 b{3.0f, 4.0f, 5.0f, 1.0f};
    auto actual = HadamardProduct(a, b);
    EXPECT_EQ(actual, Vector4(6.0f, 12.0f, 20.0f, 1.0f));
}

TEST(Vector_unit_tests, Vector4_SquareMagnitude)
{
    Vector4 a{2.0f, 2.0f, 2.0f, 2.0f};
    auto actual = SquareMagnitude(a);
    EXPECT_FLOAT_EQ(actual, 16.0f);
}

TEST(Vector_unit_tests, Vector4_Magnitude)
{
    Vector4 a{2.0f, 2.0f, 2.0f, 2.0f};
    auto actual = Magnitude(a);
    EXPECT_FLOAT_EQ(actual, 4.0f);
}

TEST(Vector_unit_tests, Vector4_Normalize)
{
    Vector4 a{2.0f, 2.0f, 2.0f, 2.0f};
    auto actual = Normalize(a);
    EXPECT_FLOAT_EQ(actual.x, 0.5f);
    EXPECT_FLOAT_EQ(actual.x, actual.y);
    EXPECT_FLOAT_EQ(actual.y, actual.z);
    EXPECT_FLOAT_EQ(actual.z, actual.w);
}

TEST(Vector_unit_tests, Vector4_Lerp)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{2.0f, 3.0f, 4.0f, 5.0f};
    float factor = 0.5f;
    auto actual = Lerp(a, b, factor);
    EXPECT_FLOAT_EQ(actual.x, 1.5f);
    EXPECT_FLOAT_EQ(actual.y, 2.5f);
    EXPECT_FLOAT_EQ(actual.z, 3.5f);
    EXPECT_FLOAT_EQ(actual.w, 4.5f);
}

TEST(Vector_unit_tests, Vector4_SquareDistance)
{
    Vector4 a{1.0f, 1.0f, 1.0f, 1.0f};
    Vector4 b{2.0f, 2.0f, 2.0f, 2.0f};
    auto actual = SquareDistance(a, b);
    EXPECT_FLOAT_EQ(actual, 4.0f);
}

TEST(Vector_unit_tests, Vector4_Distance)
{
    Vector4 a{1.0f, 1.0f, 1.0f, 1.0f};
    Vector4 b{2.0f, 2.0f, 2.0f, 2.0f};
    auto actual = Distance(a, b);
    EXPECT_FLOAT_EQ(actual, 2);
}

TEST(Vector_unit_tests, Vector4_HasAnyZeroElement)
{
    Vector4 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4 b{0.0f, 1.0f, 1.0f, 1.0f};
    Vector4 c{1.0f, 0.0f, 0.0f, 0.0f};
    EXPECT_FALSE(HasAnyZeroElement(a));
    EXPECT_TRUE(HasAnyZeroElement(b));
    EXPECT_TRUE(HasAnyZeroElement(c));
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}