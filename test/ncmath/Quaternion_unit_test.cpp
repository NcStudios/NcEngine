#include "gtest/gtest.h"
#include "ncmath/Quaternion.h"

#include <array>

constexpr float g_epsilon = 1.0e-5f;

void ExpectVectorNear(const nc::Vector3& expected,
                      const nc::Vector3& actual,
                      float epsilon = g_epsilon)
{
    EXPECT_NEAR(expected.x, actual.x, epsilon);
    EXPECT_NEAR(expected.y, actual.y, epsilon);
    EXPECT_NEAR(expected.z, actual.z, epsilon);
}

void ExpectSameRotation(const nc::Quaternion& lhs,
                        const nc::Quaternion& rhs,
                        float epsilon = g_epsilon)
{
    const auto same =
        std::abs(lhs.x - rhs.x) <= epsilon &&
        std::abs(lhs.y - rhs.y) <= epsilon &&
        std::abs(lhs.z - rhs.z) <= epsilon &&
        std::abs(lhs.w - rhs.w) <= epsilon;

    const auto opposite =
        std::abs(lhs.x + rhs.x) <= epsilon &&
        std::abs(lhs.y + rhs.y) <= epsilon &&
        std::abs(lhs.z + rhs.z) <= epsilon &&
        std::abs(lhs.w + rhs.w) <= epsilon;

    EXPECT_TRUE(same || opposite)
        << "Quaternions represent different rotations:\n"
        << "lhs = (" << lhs.x << ", " << lhs.y << ", " << lhs.z << ", " << lhs.w << ")\n"
        << "rhs = (" << rhs.x << ", " << rhs.y << ", " << rhs.z << ", " << rhs.w << ")";
}

void ExpectUnitQuaternion(const nc::Quaternion& q, float epsilon = g_epsilon)
{
    const float sqrMag =
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w;

    EXPECT_NEAR(sqrMag, 1.0f, epsilon);
}

TEST(QuaternionTest, DefaultConstructor_returnsIdentity)
{
    const auto uut = nc::Quaternion{};

    EXPECT_FLOAT_EQ(uut.x, 0.0f);
    EXPECT_FLOAT_EQ(uut.y, 0.0f);
    EXPECT_FLOAT_EQ(uut.z, 0.0f);
    EXPECT_FLOAT_EQ(uut.w, 1.0f);
}

TEST(QuaternionTest, Identity_returnsIdentity)
{
    const auto uut = nc::Quaternion::Identity();

    EXPECT_FLOAT_EQ(uut.x, 0.0f);
    EXPECT_FLOAT_EQ(uut.y, 0.0f);
    EXPECT_FLOAT_EQ(uut.z, 0.0f);
    EXPECT_FLOAT_EQ(uut.w, 1.0f);
}

TEST(QuaternionTest, Normalize_producesUnitQuaternion)
{
    const auto q = nc::Normalize(nc::Quaternion{1.0f, 2.0f, 3.0f, 4.0f});
    const auto length = std::sqrt(1.0f + 4.0f + 9.0f + 16.0f);

    ExpectUnitQuaternion(q);
    EXPECT_NEAR(q.x, 1.0f / length, g_epsilon);
    EXPECT_NEAR(q.y, 2.0f / length, g_epsilon);
    EXPECT_NEAR(q.z, 3.0f / length, g_epsilon);
    EXPECT_NEAR(q.w, 4.0f / length, g_epsilon);
}

TEST(QuaternionTest, Normalize_identity_isIdentity)
{
    const auto identity = nc::Quaternion::Identity();
    ExpectSameRotation(nc::Normalize(identity), identity);
}

TEST(QuaternionTest, FromEulerAngles_overloadsProducesSameResult)
{
    const auto angles = nc::Vector3{0.25f, -0.75f, 1.25f};
    const auto fromVector = nc::Quaternion::FromEulerAngles(angles);
    const auto fromComponents = nc::Quaternion::FromEulerAngles(angles.x, angles.y, angles.z);

    ExpectSameRotation(fromVector, fromComponents);
}

TEST(QuaternionTest, FromEulerAngles_ToEulerAngles_roundTrip)
{
    const auto expected = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto uut = nc::Quaternion::FromEulerAngles(expected);
    const auto actual = uut.ToEulerAngles();

    ExpectVectorNear(expected, actual);
}

TEST(QuaternionTest, FromEulerAngles_ToEulerAngles_roundTripSeveralRotations)
{
    constexpr auto testAngles = std::array{
        nc::Vector3{ 0.0f,  0.0f,   0.0f},
        nc::Vector3{ 0.1f,  0.2f,   0.3f},
        nc::Vector3{-0.5f,  0.75f, -1.25f},
        nc::Vector3{ 1.0f,  2.0f,   3.0f},
        nc::Vector3{-1.0f, -2.0f,  -3.0f},
        nc::Vector3{ 3.0f, -2.0f,   1.0f},
    };

    for (const auto& angles : testAngles)
    {
        const auto uut = nc::Quaternion::FromEulerAngles(angles);
        const auto convertedAngles = uut.ToEulerAngles();
        const auto reconstituted = nc::Quaternion::FromEulerAngles(convertedAngles);
        ExpectSameRotation(uut, reconstituted);
    }
}

TEST(QuaternionTest, FromEulerAngles_usesExpectedConvention)
{
    const auto angles = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto uut = nc::Quaternion::FromEulerAngles(angles);

    // DirectXMath's expected values
    EXPECT_NEAR(uut.x,  0.754934f, g_epsilon);
    EXPECT_NEAR(uut.y, -0.206149f, g_epsilon);
    EXPECT_NEAR(uut.z,  0.444435f, g_epsilon);
    EXPECT_NEAR(uut.w,  0.435953f, g_epsilon);
}

TEST(QuaternionTest, FromAxisAngle_zeroRotation_returnsIdentity)
{
    const auto uut = nc::Quaternion::FromAxisAngle(nc::Vector3::Right(), 0.0f);
    ExpectSameRotation(uut, nc::Quaternion::Identity());
}

TEST(QuaternionTest, FromAxisAngle_xAxis)
{
    const auto uut = nc::Quaternion::FromAxisAngle(nc::Vector3::Right(), 1.0f);
    const auto halfAngle = 0.5f;

    EXPECT_NEAR(uut.x, std::sin(halfAngle), g_epsilon);
    EXPECT_NEAR(uut.y, 0.0f,                g_epsilon);
    EXPECT_NEAR(uut.z, 0.0f,                g_epsilon);
    EXPECT_NEAR(uut.w, std::cos(halfAngle), g_epsilon);
}

TEST(QuaternionTest, FromAxisAngle_yYAxis)
{
    const auto uut = nc::Quaternion::FromAxisAngle(nc::Vector3::Up(), 1.0f);
    const auto halfAngle = 0.5f;

    EXPECT_NEAR(uut.x, 0.0f,                g_epsilon);
    EXPECT_NEAR(uut.y, std::sin(halfAngle), g_epsilon);
    EXPECT_NEAR(uut.z, 0.0f,                g_epsilon);
    EXPECT_NEAR(uut.w, std::cos(halfAngle), g_epsilon);
}

TEST(QuaternionTest, FromAxisAngle_roundTrip)
{
    const auto expectedAxis = nc::Normalize(nc::Vector3{1.0f, 2.0f, 3.0f});
    const auto expectedAngle = 1.25f;
    const auto uut = nc::Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualAxis = nc::Vector3{};
    auto actualAngle = 0.0f;
    uut.ToAxisAngle(&actualAxis, &actualAngle);

    ExpectVectorNear(expectedAxis, actualAxis);
    EXPECT_NEAR(expectedAngle, actualAngle, g_epsilon);

    const auto reconstituted = nc::Quaternion::FromAxisAngle(actualAxis, actualAngle);
    ExpectSameRotation(uut, reconstituted);
}

TEST(QuaternionTest, ToAxisAngle_identity_setsZeroAngle)
{
    auto axis = nc::Vector3{};
    auto angle = 0.0f;
    auto uut = nc::Quaternion::Identity();
    uut.ToAxisAngle(&axis, &angle);

    EXPECT_NEAR(angle, 0.0f, g_epsilon);
}

TEST(QuaternionTest, ToAxisAngle_preservesRotation)
{
    const auto expectedAngle = 2.0f;
    const auto expectedAxis = nc::Normalize(
        nc::Vector3{1.0f, 2.0f, 3.0f} / 4.0f
    );

    const auto uut = nc::Quaternion::FromAxisAngle(expectedAxis, expectedAngle);
    auto actualAxis = nc::Vector3{};
    auto actualAngle = 0.0f;
    uut.ToAxisAngle(&actualAxis, &actualAngle);

    EXPECT_EQ(expectedAxis, actualAxis);
    EXPECT_NEAR(actualAngle, expectedAngle, g_epsilon);

    const auto reconstituted = nc::Quaternion::FromAxisAngle(actualAxis, actualAngle);
    ExpectSameRotation(uut, reconstituted);
}

TEST(QuaternionTest, Multiply_identity_preservesRotation)
{
    const auto uut = nc::Quaternion::FromEulerAngles(nc::Vector3{0.3f, 0.7f, -1.2f});
    const auto identityLhs = nc::Multiply(nc::Quaternion::Identity(), uut);
    const auto identityRhs = nc::Multiply(uut, nc::Quaternion::Identity());

    ExpectSameRotation(identityLhs, uut);
    ExpectSameRotation(identityRhs, uut);
}

TEST(QuaternionTest, Multiply_isAssociative)
{
    const auto a = nc::Quaternion::FromEulerAngles(nc::Vector3{ 0.2f,  0.3f, 0.4f});
    const auto b = nc::Quaternion::FromEulerAngles(nc::Vector3{-0.4f,  0.5f, 0.6f});
    const auto c = nc::Quaternion::FromEulerAngles(nc::Vector3{ 0.7f, -0.2f, 0.1f});
    const auto lhs = nc::Multiply(Multiply(a, b), c);
    const auto rhs = nc::Multiply(a, Multiply(b, c));

    ExpectSameRotation(lhs, rhs);
}

TEST(QuaternionTest, Difference_withIdenticalRotations_returnsIdentity)
{
    const auto q = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.8f, 1.1f});
    const auto difference = nc::Difference(q, q);

    ExpectSameRotation(difference, nc::Quaternion::Identity());
}

TEST(QuaternionTest, Difference_withIdentity_returnsOther)
{
    const auto q = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.8f, 1.1f});
    const auto difference = nc::Difference(nc::Quaternion::Identity(), q);

    ExpectSameRotation(difference, q);
}

TEST(QuaternionTest, Slerp_factorZero_returnsLhs)
{
    const auto lhs = nc::Quaternion::FromEulerAngles(nc::Vector3{0.2f, 0.4f, 0.6f});
    const auto rhs = nc::Quaternion::FromEulerAngles(nc::Vector3{-0.5f, 0.8f, -1.0f});
    const auto actual = Slerp(lhs, rhs, 0.0f);

    ExpectSameRotation(actual, lhs);
}

TEST(QuaternionTest, Slerp_factorOne_returnsRhs)
{
    const auto lhs = nc::Quaternion::FromEulerAngles(nc::Vector3{0.2f, 0.4f, 0.6f});
    const auto rhs = nc::Quaternion::FromEulerAngles(nc::Vector3{-0.5f, 0.8f, -1.0f});
    const auto actual = nc::Slerp(lhs, rhs, 1.0f);

    ExpectSameRotation(actual, rhs);
}

TEST(QuaternionTest, Slerp_factorOneHalf_returnsMidpoint)
{
    const auto rhs = nc::Quaternion::FromAxisAngle(nc::Vector3::Right(), 3.14f);
    const auto halfway = nc::Slerp(nc::Quaternion::Identity(), rhs, 0.5f);
    const auto expected = nc::Quaternion::FromAxisAngle(nc::Vector3::Right(), 3.14f * 0.5f);

    ExpectSameRotation(halfway, expected);
}

TEST(QuaternionTest, Slerp_returnsUnitQuaternion)
{
    const auto lhs = nc::Quaternion::FromEulerAngles(nc::Vector3{0.2f, 0.4f, 0.6f});
    const auto rhs = nc::Quaternion::FromEulerAngles(nc::Vector3{-0.5f, 0.8f, -1.0f});
    const auto result = nc::Slerp(lhs, rhs, 0.37f);

    ExpectUnitQuaternion(result);
}

TEST(QuaternionTest, Slerp_equivalentOppositeSignedQuaternions)
{
    // q == -q
    const auto q = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.7f, 1.2f});
    const auto negated = nc::Quaternion{-q.x, -q.y, -q.z, -q.w};
    const auto actual = nc::Slerp(q, negated, 0.5f);

    ExpectSameRotation(actual, q);
}

TEST(QuaternionTest, Scale_factorZero_returnsIdentity)
{
    const auto uut = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.7f, 1.2f});
    ExpectSameRotation(nc::Scale(uut, 0.0f), nc::Quaternion::Identity());
}

TEST(QuaternionTest, Scale_factorOne_returnsOriginal)
{
    const auto uut = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.7f, 1.2f});
    ExpectSameRotation(nc::Scale(uut, 1.0f), uut);
}

TEST(QuaternionTest, Scale_factorOneHalf_returnsHalf)
{
    const auto uut = nc::Quaternion::FromAxisAngle(nc::Vector3::Up(), 3.14f);
    const auto actual = nc::Scale(uut, 0.5f);
    const auto expected = nc::Quaternion::FromAxisAngle(nc::Vector3::Up(), 3.14f * 0.5f);

    ExpectSameRotation(expected, actual);
}

TEST(QuaternionTest, Scale_producesUnitQuaternion)
{
    const auto uut = nc::Quaternion::FromEulerAngles(nc::Vector3{0.4f, -0.7f, 1.2f});
    for (const auto factor : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f})
    {
        ExpectUnitQuaternion(nc::Scale(uut, factor));
    }
}
