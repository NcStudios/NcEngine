#include "JoltApiFixture.inl"
#include "physics2/jolt/ConstraintFactory.h"

#include "physics2/jolt/Conversion.h"
#include "ncutility/ScopeExit.h"

#include "Jolt/Physics/Constraints/DistanceConstraint.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/HingeConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/Constraints/SliderConstraint.h"
#include "Jolt/Physics/Constraints/SwingTwistConstraint.h"

// note: Jolt Constraint settings are always returned in local space. This also means the value of mSpace doesn't
//       map to the ConstraintSpace provided in the input constraint info.

class ConstraintFactoryTest : public JoltApiFixture
{
    protected:
        nc::physics::ConstraintFactory uut;

        ConstraintFactoryTest()
            : uut{joltApi.physicsSystem}
        {
        }
};

TEST_F(ConstraintFactoryTest, MakeConstraint_FixedConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::FixedConstraintInfo{
        .ownerPosition = nc::Vector3{1.0f, 2.0f, 3.0f},
        .ownerRight = nc::Vector3::Right(),
        .ownerUp = nc::Vector3::Up(),
        .targetPosition = nc::Vector3{4.0f, 5.0f, 6.0f},
        .targetRight = nc::Vector3::Front(),
        .targetUp = nc::Vector3::Down()
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Fixed, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::FixedConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.ownerRight, nc::physics::ToVector3(actualSettings->mAxisX1));
    EXPECT_EQ(inConstraint.ownerUp, nc::physics::ToVector3(actualSettings->mAxisY1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.targetRight, nc::physics::ToVector3(actualSettings->mAxisX2));
    EXPECT_EQ(inConstraint.targetUp, nc::physics::ToVector3(actualSettings->mAxisY2));
    // detectFromPosition is not saved to the settings
}

TEST_F(ConstraintFactoryTest, MakeConstraint_FixedConstraint_oneBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto dummy = &JPH::Body::sFixedToWorld;
    SCOPE_EXIT
    (
        DestroyBody(body1);
    );

    const auto inConstraint = nc::physics::FixedConstraintInfo{
        .ownerPosition = nc::Vector3{1.0f, 2.0f, 3.0f},
        .ownerRight = nc::Vector3::Right(),
        .ownerUp = nc::Vector3::Up(),
        .targetPosition = nc::Vector3{4.0f, 5.0f, 6.0f},
        .targetRight = nc::Vector3::Front(),
        .targetUp = nc::Vector3::Down()
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *dummy);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Fixed, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::FixedConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.ownerRight, nc::physics::ToVector3(actualSettings->mAxisX1));
    EXPECT_EQ(inConstraint.ownerUp, nc::physics::ToVector3(actualSettings->mAxisY1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.targetRight, nc::physics::ToVector3(actualSettings->mAxisX2));
    EXPECT_EQ(inConstraint.targetUp, nc::physics::ToVector3(actualSettings->mAxisY2));
    // detectFromPosition is not saved to the settings
}

TEST_F(ConstraintFactoryTest, MakeConstraint_PointConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::PointConstraintInfo{
        .ownerPosition = nc::Vector3{1.0f, 2.0f, 3.0f},
        .targetPosition = nc::Vector3{-1.0f, -2.0f, -3.0f}
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Point, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::PointConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
}

TEST_F(ConstraintFactoryTest, MakeConstraint_PointConstraint_oneBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto dummy = &JPH::Body::sFixedToWorld;
    SCOPE_EXIT
    (
        DestroyBody(body1);
    );

    const auto inConstraint = nc::physics::PointConstraintInfo{
        .ownerPosition = nc::Vector3{1.0f, 2.0f, 3.0f},
        .targetPosition = nc::Vector3{-1.0f, -2.0f, -3.0f}
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *dummy);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Point, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::PointConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
}

TEST_F(ConstraintFactoryTest, MakeConstraint_DistanceConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::DistanceConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .minLimit = 2.0f,
        .maxLimit = 2.0f,
        .springSettings = nc::physics::SpringSettings{
            .frequency = 5.0f,
            .damping = 0.2f
        }
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Distance, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::DistanceConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.minLimit, actualSettings->mMinDistance);
    EXPECT_EQ(inConstraint.maxLimit, actualSettings->mMaxDistance);
    const auto& actualSpring = actualSettings->mLimitsSpringSettings;
    EXPECT_EQ(JPH::ESpringMode::FrequencyAndDamping, actualSpring.mMode);
    EXPECT_EQ(inConstraint.springSettings.frequency, actualSpring.mFrequency);
    EXPECT_EQ(inConstraint.springSettings.damping, actualSpring.mDamping);
}

TEST_F(ConstraintFactoryTest, MakeConstraint_DistanceConstraint_badValues_throws)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    auto inConstraint = nc::physics::DistanceConstraintInfo{};

    inConstraint.minLimit = -1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // minLimit must be >= 0

    inConstraint.minLimit = 2.0f;
    inConstraint.maxLimit = 1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // maxLimit must be >= minLimit
}

TEST_F(ConstraintFactoryTest, MakeConstraint_HingeConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::HingeConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .hingeAxis = nc::Normalize(nc::Vector3::One()),
        .minLimit = -std::numbers::pi_v<float>,
        .maxLimit = std::numbers::pi_v<float>,
        .maxFrictionTorque = 100.0f,
        .springSettings = nc::physics::SpringSettings{
            .frequency = 20.0f,
            .damping = 2.0f
        }
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Hinge, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::HingeConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.hingeAxis, nc::physics::ToVector3(actualSettings->mHingeAxis1));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mHingeAxis1.Dot(actualSettings->mNormalAxis1));
    EXPECT_EQ(inConstraint.hingeAxis, nc::physics::ToVector3(actualSettings->mHingeAxis2));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mHingeAxis2.Dot(actualSettings->mNormalAxis2));
    EXPECT_EQ(inConstraint.minLimit, actualSettings->mLimitsMin);
    EXPECT_EQ(inConstraint.maxLimit, actualSettings->mLimitsMax);
    EXPECT_EQ(inConstraint.maxFrictionTorque, actualSettings->mMaxFrictionTorque);
    const auto& actualSpring = actualSettings->mLimitsSpringSettings;
    EXPECT_EQ(JPH::ESpringMode::FrequencyAndDamping, actualSpring.mMode);
    EXPECT_EQ(inConstraint.springSettings.frequency, actualSpring.mFrequency);
    EXPECT_EQ(inConstraint.springSettings.damping, actualSpring.mDamping);
}

TEST_F(ConstraintFactoryTest, MakeConstraint_HingeConstraint_badValues_throws)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    auto inConstraint = nc::physics::HingeConstraintInfo{};

    inConstraint.minLimit = 1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // minLimit must be <= 0

    inConstraint.minLimit = -2.0f;
    inConstraint.maxLimit = -1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // maxLimit must be >= 0

    inConstraint.minLimit = 0.0f;
    inConstraint.maxLimit = 0.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // total limit must be > 0
}

TEST_F(ConstraintFactoryTest, MakeConstraint_SliderConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::SliderConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .sliderAxis = nc::Normalize(nc::Vector3::One()),
        .minLimit = -2.0f,
        .maxLimit = 2.0f,
        .maxFrictionForce = 100.0f,
        .springSettings = nc::physics::SpringSettings{
            .frequency = 2.0f,
            .damping = 0.1f
        }
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Slider, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::SliderConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.sliderAxis, nc::physics::ToVector3(actualSettings->mSliderAxis1));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mSliderAxis1.Dot(actualSettings->mNormalAxis1)); // is orthogonal
    EXPECT_EQ(inConstraint.sliderAxis, nc::physics::ToVector3(actualSettings->mSliderAxis2));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mSliderAxis2.Dot(actualSettings->mNormalAxis2)); // is orthogonal
    EXPECT_EQ(inConstraint.minLimit, actualSettings->mLimitsMin);
    EXPECT_EQ(inConstraint.maxLimit, actualSettings->mLimitsMax);
    EXPECT_EQ(inConstraint.maxFrictionForce, actualSettings->mMaxFrictionForce);
    const auto& actualSpring = actualSettings->mLimitsSpringSettings;
    EXPECT_EQ(JPH::ESpringMode::FrequencyAndDamping, actualSpring.mMode);
    EXPECT_EQ(inConstraint.springSettings.frequency, actualSpring.mFrequency);
    EXPECT_EQ(inConstraint.springSettings.damping, actualSpring.mDamping);
}

TEST_F(ConstraintFactoryTest, MakeConstraint_SliderConstraint_badValues_throws)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    auto inConstraint = nc::physics::SliderConstraintInfo{};

    inConstraint.minLimit = 1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // minLimit must be <= 0

    inConstraint.minLimit = -2.0f;
    inConstraint.maxLimit = -1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // maxLimit must be >= 0

    inConstraint.minLimit = 0.0f;
    inConstraint.maxLimit = 0.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception); // total limit must be > 0
}

TEST_F(ConstraintFactoryTest, MakeConstraint_SwingTwistConstraint_twoBody_setsExpectedValues)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::SwingTwistConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .ownerTwistAxis = nc::Vector3::Up(),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .targetTwistAxis = nc::Vector3::Right(),
        .swingLimit = 3.14f,
        .twistLimit = 3.14f,
        .maxFrictionTorque = 100.0f
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::SwingTwist, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::SwingTwistConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.ownerPosition, nc::physics::ToVector3(actualSettings->mPosition1));
    EXPECT_EQ(inConstraint.ownerTwistAxis, nc::physics::ToVector3(actualSettings->mTwistAxis1));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mTwistAxis1.Dot(actualSettings->mPlaneAxis1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPosition2));
    EXPECT_EQ(inConstraint.targetTwistAxis, nc::physics::ToVector3(actualSettings->mTwistAxis2));
    EXPECT_FLOAT_EQ(0.0f, actualSettings->mTwistAxis2.Dot(actualSettings->mPlaneAxis2));
    EXPECT_EQ(JPH::ESwingType::Cone, actualSettings->mSwingType);
    EXPECT_FLOAT_EQ(inConstraint.swingLimit * 0.5f, actualSettings->mPlaneHalfConeAngle);
    EXPECT_FLOAT_EQ(inConstraint.swingLimit * 0.5f, actualSettings->mNormalHalfConeAngle);
    EXPECT_FLOAT_EQ(-inConstraint.twistLimit, actualSettings->mTwistMinAngle);
    EXPECT_FLOAT_EQ(inConstraint.twistLimit, actualSettings->mTwistMaxAngle);
    EXPECT_FLOAT_EQ(inConstraint.maxFrictionTorque, actualSettings->mMaxFrictionTorque);
}

TEST_F(ConstraintFactoryTest, MakeConstraint_SwingTwistConstraint_badValues_throws)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    auto inConstraint = nc::physics::SwingTwistConstraintInfo{};

    // 0 <= limits <= pi
    inConstraint.swingLimit = -1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception);

    inConstraint.swingLimit = 3.15f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception);

    inConstraint.swingLimit = 0.0f;
    inConstraint.twistLimit = -1.0f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception);

    inConstraint.twistLimit = 3.15f;
    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception);
}
