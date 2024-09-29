#include "gtest/gtest.h"
#include "physics/jolt/Conversion.h"

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
    EXPECT_EQ(JPH::EMotionType::Dynamic, nc::physics::ToMotionType(nc::BodyType::Dynamic));
    EXPECT_EQ(JPH::EMotionType::Static, nc::physics::ToMotionType(nc::BodyType::Static));
    EXPECT_EQ(JPH::EMotionType::Kinematic, nc::physics::ToMotionType(nc::BodyType::Kinematic));
}

TEST(JoltConversionTest, ToObjectLayer_determinesLayer)
{
    EXPECT_EQ(nc::physics::ObjectLayer::Dynamic, nc::physics::ToObjectLayer(nc::BodyType::Dynamic, false));
    EXPECT_EQ(nc::physics::ObjectLayer::Dynamic, nc::physics::ToObjectLayer(nc::BodyType::Kinematic, false));
    EXPECT_EQ(nc::physics::ObjectLayer::Static, nc::physics::ToObjectLayer(nc::BodyType::Static, false));
    EXPECT_EQ(nc::physics::ObjectLayer::Trigger, nc::physics::ToObjectLayer(nc::BodyType::Dynamic, true));
    EXPECT_EQ(nc::physics::ObjectLayer::Trigger, nc::physics::ToObjectLayer(nc::BodyType::Kinematic, true));
    EXPECT_EQ(nc::physics::ObjectLayer::Trigger, nc::physics::ToObjectLayer(nc::BodyType::Static, true));
}

TEST(JoltConversionTest, ToSpringSettings_convertsSettings)
{
    const auto expected = nc::SpringSettings{
        .frequency = 10.0f,
        .damping = 1.0f
    };

    const auto actual = nc::physics::ToSpringSettings(expected);
    EXPECT_EQ(JPH::ESpringMode::FrequencyAndDamping, actual.mMode);
    EXPECT_FLOAT_EQ(expected.frequency, actual.mFrequency);
    EXPECT_FLOAT_EQ(expected.damping, actual.mDamping);
}

TEST(JoltConversionTest, ToAllowedDOFs_convertsFlags)
{
    using namespace nc::physics;
    constexpr auto allTranslation = JPH::EAllowedDOFs::TranslationX |
                                    JPH::EAllowedDOFs::TranslationY |
                                    JPH::EAllowedDOFs::TranslationZ;

    constexpr auto allRotation = JPH::EAllowedDOFs::RotationX |
                                 JPH::EAllowedDOFs::RotationY |
                                 JPH::EAllowedDOFs::RotationZ;

    EXPECT_EQ(JPH::EAllowedDOFs::All, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::All));
    EXPECT_EQ(JPH::EAllowedDOFs::TranslationX, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::TranslationX));
    EXPECT_EQ(JPH::EAllowedDOFs::TranslationY, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::TranslationY));
    EXPECT_EQ(JPH::EAllowedDOFs::TranslationZ, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::TranslationZ));
    EXPECT_EQ(allTranslation, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::Translation));
    EXPECT_EQ(JPH::EAllowedDOFs::RotationX, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::RotationX));
    EXPECT_EQ(JPH::EAllowedDOFs::RotationY, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::RotationY));
    EXPECT_EQ(JPH::EAllowedDOFs::RotationZ, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::RotationZ));
    EXPECT_EQ(allRotation, nc::physics::ToAllowedDOFs(nc::DegreeOfFreedom::Rotation));
}
