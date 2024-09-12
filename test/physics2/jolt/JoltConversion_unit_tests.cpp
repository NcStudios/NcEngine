#include "gtest/gtest.h"
#include "physics2/jolt/Conversion.h"

TEST(JoltConversionTest, XMVector3_roundTrip_preservesValue)
{
    const auto expectedVector = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
    const auto joltVector = nc::physics::ToJoltVec3(expectedVector);
    const auto actualVector = nc::physics::ToXMVector(joltVector);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetX(expectedVector), DirectX::XMVectorGetX(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetY(expectedVector), DirectX::XMVectorGetY(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetZ(expectedVector), DirectX::XMVectorGetZ(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetW(expectedVector), DirectX::XMVectorGetW(actualVector));
}

TEST(JoltConversionTest, XMVector3Homogeneous_roundTrip_preservesValue)
{
    const auto expectedVector = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 1.0f);
    const auto joltVector = nc::physics::ToJoltVec3(expectedVector);
    const auto actualVector = nc::physics::ToXMVectorHomogeneous(joltVector);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetX(expectedVector), DirectX::XMVectorGetX(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetY(expectedVector), DirectX::XMVectorGetY(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetZ(expectedVector), DirectX::XMVectorGetZ(actualVector));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetW(expectedVector), DirectX::XMVectorGetW(actualVector));
}

TEST(JoltConversionTest, Vector3_roundTrip_preservesValue)
{
    const auto expectedVector = nc::Vector3{1.0f, 2.0f, 3.0f};
    const auto joltVector = nc::physics::ToJoltVec3(expectedVector);
    const auto actualVector = nc::physics::ToVector3(joltVector);
    EXPECT_EQ(expectedVector, actualVector);
}

TEST(JoltConversionTest, XMQuaternion_roundTrip_preservesValue)
{
    const auto expectedQuat = DirectX::XMQuaternionRotationRollPitchYaw(3.14f, 0.0f, 1.07f);
    const auto joltQuat = nc::physics::ToJoltQuaternion(expectedQuat);
    const auto actualQuat = nc::physics::ToXMQuaternion(joltQuat);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetX(expectedQuat), DirectX::XMVectorGetX(actualQuat));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetY(expectedQuat), DirectX::XMVectorGetY(actualQuat));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetZ(expectedQuat), DirectX::XMVectorGetZ(actualQuat));
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetW(expectedQuat), DirectX::XMVectorGetW(actualQuat));
}

TEST(JoltConversionTest, Quaternion_roundTrip_preservesValue)
{
    const auto expectedQuat = nc::Quaternion::FromEulerAngles(3.14f, 0.0f, 1.07f);
    const auto joltQuat = nc::physics::ToJoltQuaternion(expectedQuat);
    const auto actualQuat = nc::physics::ToQuaternion(joltQuat);
    EXPECT_EQ(expectedQuat, actualQuat);
}

TEST(JoltConversionTest, ToMotionQuality_convertsValue)
{
    EXPECT_EQ(JPH::EMotionQuality::Discrete, nc::physics::ToMotionQuality(false));
    EXPECT_EQ(JPH::EMotionQuality::LinearCast, nc::physics::ToMotionQuality(true));
}

TEST(JoltConversionTest, ToMotionType_convertsBodyType)
{
    EXPECT_EQ(JPH::EMotionType::Dynamic, ToMotionType(nc::physics::BodyType::Dynamic));
    EXPECT_EQ(JPH::EMotionType::Static, ToMotionType(nc::physics::BodyType::Static));
    EXPECT_EQ(JPH::EMotionType::Kinematic, ToMotionType(nc::physics::BodyType::Kinematic));
}
