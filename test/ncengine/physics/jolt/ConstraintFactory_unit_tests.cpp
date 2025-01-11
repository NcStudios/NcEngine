#include "JoltApiFixture.inl"
#include "physics/jolt/ConstraintFactory.h"

#include "physics/jolt/Conversion.h"
#include "ncutility/ScopeExit.h"

#include "Jolt/Physics/Constraints/DistanceConstraint.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/HingeConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"
#include "Jolt/Physics/Constraints/SliderConstraint.h"
#include "Jolt/Physics/Constraints/SwingTwistConstraint.h"
#include "Jolt/Physics/Vehicle/VehicleConstraint.h"
#include "Jolt/Physics/Vehicle/WheeledVehicleController.h"

#include <ranges>

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

    const auto inConstraint = nc::FixedConstraintInfo{
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

    const auto inConstraint = nc::FixedConstraintInfo{
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

    const auto inConstraint = nc::PointConstraintInfo{
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

    const auto inConstraint = nc::PointConstraintInfo{
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

    const auto inConstraint = nc::DistanceConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .minLimit = 2.0f,
        .maxLimit = 2.0f,
        .springSettings = nc::SpringSettings{
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

    auto inConstraint = nc::DistanceConstraintInfo{};

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

    const auto inConstraint = nc::HingeConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .ownerHingeAxis = nc::Vector3::Right(),
        .ownerNormalAxis = nc::Vector3::Up(),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .targetHingeAxis = nc::Vector3::Down(),
        .targetNormalAxis = -nc::Vector3::Right(),
        .minLimit = -std::numbers::pi_v<float>,
        .maxLimit = std::numbers::pi_v<float>,
        .maxFrictionTorque = 100.0f,
        .springSettings = nc::SpringSettings{
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
    EXPECT_EQ(inConstraint.ownerHingeAxis, nc::physics::ToVector3(actualSettings->mHingeAxis1));
    EXPECT_EQ(inConstraint.ownerNormalAxis, nc::physics::ToVector3(actualSettings->mNormalAxis1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.targetHingeAxis, nc::physics::ToVector3(actualSettings->mHingeAxis2));
    EXPECT_EQ(inConstraint.targetNormalAxis, nc::physics::ToVector3(actualSettings->mNormalAxis2));
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

    auto inConstraint = nc::HingeConstraintInfo{};

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

    const auto inConstraint = nc::SliderConstraintInfo{
        .ownerPosition = nc::Vector3::Splat(2.0f),
        .ownerSliderAxis = Normalize(nc::Vector3::Splat(1.0f)),
        .ownerNormalAxis = OrthogonalTo(Normalize(nc::Vector3::Splat(1.0f))),
        .targetPosition = nc::Vector3::Splat(3.0f),
        .targetSliderAxis = Normalize(nc::Vector3::Splat(-11.0f)),
        .targetNormalAxis = OrthogonalTo(Normalize(nc::Vector3::Splat(-1.0f))),
        .minLimit = -2.0f,
        .maxLimit = 2.0f,
        .maxFrictionForce = 100.0f,
        .springSettings = nc::SpringSettings{
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
    EXPECT_EQ(inConstraint.ownerSliderAxis, nc::physics::ToVector3(actualSettings->mSliderAxis1));
    EXPECT_EQ(inConstraint.ownerNormalAxis, nc::physics::ToVector3(actualSettings->mNormalAxis1));
    EXPECT_EQ(inConstraint.targetPosition, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.targetSliderAxis, nc::physics::ToVector3(actualSettings->mSliderAxis2));
    EXPECT_EQ(inConstraint.targetNormalAxis, nc::physics::ToVector3(actualSettings->mNormalAxis2));
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

    auto inConstraint = nc::SliderConstraintInfo{};

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

    const auto inConstraint = nc::SwingTwistConstraintInfo{
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

    auto inConstraint = nc::SwingTwistConstraintInfo{};

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

TEST_F(ConstraintFactoryTest, MakeVehicleConstraint_validCall_setsExpectedValues)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{
        .orientation = nc::VehicleOrientation{},
        .engine = nc::VehicleEngine{},
        .transmission = nc::VehicleTransmission{},
        .wheelAssemblies = {
            nc::WheelAssembly{},                                                 // 2 wheels, powered
            nc::WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()},     // 1 wheel, powered
            nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()}  // 2 wheels, unpowered
        }
    };

    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());

    EXPECT_EQ(infoIn.orientation.up, nc::physics::ToVector3(actualConstraint->GetLocalUp()));
    EXPECT_EQ(infoIn.orientation.forward, nc::physics::ToVector3(actualConstraint->GetLocalForward()));

    {
        const auto& expected = infoIn.engine;
        const auto& actual = actualController->GetEngine();
        EXPECT_FLOAT_EQ(expected.maxTorque, actual.mMaxTorque);
        EXPECT_FLOAT_EQ(expected.minRPM, actual.mMinRPM);
        EXPECT_FLOAT_EQ(expected.maxRPM, actual.mMaxRPM);
        EXPECT_FLOAT_EQ(expected.inertia, actual.mInertia);
        EXPECT_FLOAT_EQ(expected.damping, actual.mAngularDamping);
    }

    {
        const auto& expected = infoIn.transmission;
        const auto& actual = actualController->GetTransmission();
        EXPECT_TRUE(std::ranges::equal(expected.gears, actual.mGearRatios));
        EXPECT_TRUE(std::ranges::equal(expected.reverseGears, actual.mReverseGearRatios));
        EXPECT_FLOAT_EQ(expected.shiftTime, actual.mSwitchTime);
        EXPECT_FLOAT_EQ(expected.shiftLatency, actual.mSwitchLatency);
        EXPECT_FLOAT_EQ(expected.shiftUpRPM, actual.mShiftUpRPM);
        EXPECT_FLOAT_EQ(expected.shiftDownRPM, actual.mShiftDownRPM);
        EXPECT_FLOAT_EQ(expected.clutchRelease, actual.mClutchReleaseTime);
        EXPECT_FLOAT_EQ(expected.clutchStrength, actual.mClutchStrength);
    }

    {
        const auto& actual = actualController->GetDifferentials();
        EXPECT_EQ(2, actual.size());
        const auto& expectedFirst = infoIn.wheelAssemblies.at(0).differential;
        const auto& actualFirst = actual.at(0);
        EXPECT_EQ(0, actualFirst.mLeftWheel);
        EXPECT_EQ(1, actualFirst.mRightWheel);
        EXPECT_FLOAT_EQ(expectedFirst.ratio, actualFirst.mDifferentialRatio);
        EXPECT_FLOAT_EQ(expectedFirst.limitedSlipRatio, actualFirst.mLimitedSlipRatio);
        EXPECT_FLOAT_EQ(0.5f, actualFirst.mLeftRightSplit);
        EXPECT_FLOAT_EQ(0.5f, actualFirst.mEngineTorqueRatio);

        const auto& expectedSecond = infoIn.wheelAssemblies.at(1).differential;
        const auto& actualSecond = actual.at(1);
        EXPECT_EQ(2, actualSecond.mLeftWheel);
        EXPECT_EQ(-1, actualSecond.mRightWheel);
        EXPECT_FLOAT_EQ(expectedSecond.ratio, actualSecond.mDifferentialRatio);
        EXPECT_FLOAT_EQ(expectedSecond.limitedSlipRatio, actualSecond.mLimitedSlipRatio);
        EXPECT_FLOAT_EQ(0.0f, actualSecond.mLeftRightSplit);
        EXPECT_FLOAT_EQ(0.5f, actualSecond.mEngineTorqueRatio);
    }

    {
        const auto& actualWheels = actualConstraint->GetWheels();
        EXPECT_EQ(5, actualWheels.size());
        const auto expectedMounts = std::array{
            &infoIn.wheelAssemblies.at(0).leftWheel,
            &infoIn.wheelAssemblies.at(0).rightWheel,
            &infoIn.wheelAssemblies.at(1).leftWheel,
            &infoIn.wheelAssemblies.at(2).leftWheel,
            &infoIn.wheelAssemblies.at(2).rightWheel,
        };

        const auto expectedSpecs = std::array{
            &infoIn.wheelAssemblies.at(0).wheelSpec,
            &infoIn.wheelAssemblies.at(0).wheelSpec,
            &infoIn.wheelAssemblies.at(1).wheelSpec,
            &infoIn.wheelAssemblies.at(2).wheelSpec,
            &infoIn.wheelAssemblies.at(2).wheelSpec,
        };

        const auto expectedSuspension = std::array{
            &infoIn.wheelAssemblies.at(0).suspension,
            &infoIn.wheelAssemblies.at(0).suspension,
            &infoIn.wheelAssemblies.at(1).suspension,
            &infoIn.wheelAssemblies.at(2).suspension,
            &infoIn.wheelAssemblies.at(2).suspension,
        };

        for (auto i = 0; i < 5; ++i)
        {
            const auto actual = actualWheels.at(i);
            const auto& settings = static_cast<const JPH::WheelWV*>(actual)->GetSettings();

            const auto mount = expectedMounts.at(i);
            EXPECT_EQ(i, mount->id); // factory should write back index to mount
            EXPECT_EQ(mount->position, nc::physics::ToVector3(settings->mPosition));
            EXPECT_EQ(mount->up, nc::physics::ToVector3(settings->mWheelUp));
            EXPECT_EQ(mount->forward, nc::physics::ToVector3(settings->mWheelForward));
            EXPECT_EQ(mount->suspensionAxis, nc::physics::ToVector3(settings->mSuspensionDirection));
            EXPECT_EQ(mount->steeringAxis, nc::physics::ToVector3(settings->mSteeringAxis));

            const auto spec = expectedSpecs.at(i);
            EXPECT_FLOAT_EQ(spec->radius, settings->mRadius);
            EXPECT_FLOAT_EQ(spec->width, settings->mWidth);
            EXPECT_FLOAT_EQ(spec->maxSteerAngle, settings->mMaxSteerAngle);
            EXPECT_FLOAT_EQ(spec->inertia, settings->mInertia);
            EXPECT_FLOAT_EQ(spec->damping, settings->mAngularDamping);
            EXPECT_FLOAT_EQ(spec->maxBrakeTorque, settings->mMaxBrakeTorque);
            EXPECT_FLOAT_EQ(spec->maxHandBrakeTorque, settings->mMaxHandBrakeTorque);

            const auto suspension = expectedSuspension.at(i);
            EXPECT_FLOAT_EQ(suspension->minLength, settings->mSuspensionMinLength);
            EXPECT_FLOAT_EQ(suspension->maxLength, settings->mSuspensionMaxLength);
            EXPECT_FLOAT_EQ(suspension->spring.frequency, settings->mSuspensionSpring.mFrequency);
            EXPECT_FLOAT_EQ(suspension->spring.damping, settings->mSuspensionSpring.mDamping);
        }
    }
}

TEST_F(ConstraintFactoryTest, AddWheelAssembly_validCall_updatesConstraint)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{};
    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());
    const auto& actualDifferentials = actualController->GetDifferentials();
    const auto& actualWheels = actualConstraint->GetWheels();

    {
        // verify initial state
        ASSERT_EQ(0, infoIn.wheelAssemblies.at(0).leftWheel.id);
        ASSERT_EQ(1, infoIn.wheelAssemblies.at(0).rightWheel.id);
        ASSERT_EQ(2, actualWheels.size());
        ASSERT_EQ(1, actualDifferentials.size());
        const auto& initialDifferential = actualDifferentials.at(0);
        ASSERT_EQ(0, initialDifferential.mLeftWheel);
        ASSERT_EQ(1, initialDifferential.mRightWheel);
        ASSERT_FLOAT_EQ(0.5f, initialDifferential.mLeftRightSplit);
        ASSERT_FLOAT_EQ(1.0f, initialDifferential.mEngineTorqueRatio);
    }

    // update vehicle for total of 3 assemblies, 2 differentials, 6 wheels
    auto newAssembly1 = nc::WheelAssembly{};
    auto newAssembly2 = nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()};
    nc::physics::AddWheelAssembly(newAssembly1, *actualConstraint, *actualController);
    nc::physics::AddWheelAssembly(newAssembly2, *actualConstraint, *actualController);

    EXPECT_EQ(6, actualWheels.size());
    EXPECT_EQ(0, infoIn.wheelAssemblies.at(0).leftWheel.id);
    EXPECT_EQ(1, infoIn.wheelAssemblies.at(0).rightWheel.id);
    EXPECT_EQ(2, newAssembly1.leftWheel.id);
    EXPECT_EQ(3, newAssembly1.rightWheel.id);
    EXPECT_EQ(4, newAssembly2.leftWheel.id);
    EXPECT_EQ(5, newAssembly2.rightWheel.id);

    EXPECT_EQ(2, actualDifferentials.size());
    const auto& originalDifferential = actualDifferentials.at(0);
    EXPECT_EQ(0, originalDifferential.mLeftWheel);
    EXPECT_EQ(1, originalDifferential.mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, originalDifferential.mLeftRightSplit);
    EXPECT_FLOAT_EQ(0.5f, originalDifferential.mEngineTorqueRatio);

    const auto& newDifferential = actualDifferentials.at(1);
    EXPECT_EQ(2, newDifferential.mLeftWheel);
    EXPECT_EQ(3, newDifferential.mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, newDifferential.mLeftRightSplit);
    EXPECT_FLOAT_EQ(0.5f, newDifferential.mEngineTorqueRatio);
}

TEST_F(ConstraintFactoryTest, AddWheelAssembly_bothWheelsDisabled_throws)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{};
    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());
    auto bad = nc::WheelAssembly{
        .leftWheel = nc::WheelMount::MakeDisabled(),
        .rightWheel = nc::WheelMount::MakeDisabled()
    };

    EXPECT_THROW(nc::physics::AddWheelAssembly(bad, *actualConstraint, *actualController), nc::NcError);
}

TEST_F(ConstraintFactoryTest, RemoveWheelAssembly_updatesIndices)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{.leftWheel = nc::WheelMount::MakeDisabled()},
            nc::WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()},
            nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()},
            nc::WheelAssembly{},
        }
    };

    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());
    const auto& actualWheels = actualConstraint->GetWheels();
    const auto& actualDifferentials = actualController->GetDifferentials();

    ASSERT_EQ(6, actualWheels.size());
    ASSERT_EQ(3, actualDifferentials.size());

    nc::physics::RemoveWheelAssembly(0, infoIn.wheelAssemblies, *actualConstraint, *actualController);
    // expected:
    //   WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()}
    //   WheelAssembly{.differential = nc::Differential::MakeDisabled()}
    //   WheelAssembly{}
    EXPECT_EQ(5, actualWheels.size());
    EXPECT_EQ(2, actualDifferentials.size());
    EXPECT_EQ(0, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(-1, actualDifferentials.at(0).mRightWheel);
    EXPECT_EQ(3, actualDifferentials.at(1).mLeftWheel);
    EXPECT_EQ(4, actualDifferentials.at(1).mRightWheel);

    EXPECT_EQ(3, infoIn.wheelAssemblies.size());
    EXPECT_EQ(0, infoIn.wheelAssemblies.at(0).leftWheel.id);
    EXPECT_EQ(-1, infoIn.wheelAssemblies.at(0).rightWheel.id);
    EXPECT_EQ(1, infoIn.wheelAssemblies.at(1).leftWheel.id);
    EXPECT_EQ(2, infoIn.wheelAssemblies.at(1).rightWheel.id);
    EXPECT_EQ(3, infoIn.wheelAssemblies.at(2).leftWheel.id);
    EXPECT_EQ(4, infoIn.wheelAssemblies.at(2).rightWheel.id);

    nc::physics::RemoveWheelAssembly(2, infoIn.wheelAssemblies, *actualConstraint, *actualController);
    // expected:
    //   WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()}
    //   WheelAssembly{.differential = nc::Differential::MakeDisabled()}
    EXPECT_EQ(3, actualWheels.size());
    EXPECT_EQ(1, actualDifferentials.size());
    EXPECT_EQ(0, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(-1, actualDifferentials.at(0).mRightWheel);

    EXPECT_EQ(2, infoIn.wheelAssemblies.size());
    EXPECT_EQ(0, infoIn.wheelAssemblies.at(0).leftWheel.id);
    EXPECT_EQ(-1, infoIn.wheelAssemblies.at(0).rightWheel.id);
    EXPECT_EQ(1, infoIn.wheelAssemblies.at(1).leftWheel.id);
    EXPECT_EQ(2, infoIn.wheelAssemblies.at(1).rightWheel.id);

    nc::physics::RemoveWheelAssembly(1, infoIn.wheelAssemblies, *actualConstraint, *actualController);
    // expected:
    //   WheelAssembly{.rightWheel = nc::WheelMount::MakeDisabled()}
    EXPECT_EQ(1, actualWheels.size());
    EXPECT_EQ(1, actualDifferentials.size());
    EXPECT_EQ(0, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(-1, actualDifferentials.at(0).mRightWheel);

    EXPECT_EQ(1, infoIn.wheelAssemblies.size());
    EXPECT_EQ(0, infoIn.wheelAssemblies.at(0).leftWheel.id);
    EXPECT_EQ(-1, infoIn.wheelAssemblies.at(0).rightWheel.id);
}

TEST_F(ConstraintFactoryTest, ModifyWheelAssembly_updatesWheelProperties)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{};
    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());

    auto& target = infoIn.wheelAssemblies.at(0);
    target.leftWheel.up = nc::Vector3::Front();
    target.leftWheel.forward = nc::Vector3::Up();
    target.leftWheel.suspensionAxis = nc::Vector3::Front();
    nc::physics::ModifyWheelAssembly(target, *actualConstraint, *actualController);

    const auto& actualWheels = actualConstraint->GetWheels();
    const auto actualSettings = static_cast<const JPH::WheelWV*>(actualWheels.at(0))->GetSettings();
    EXPECT_EQ(target.leftWheel.up, nc::physics::ToVector3(actualSettings->mWheelUp));
    EXPECT_EQ(target.leftWheel.forward, nc::physics::ToVector3(actualSettings->mWheelForward));
    EXPECT_EQ(target.leftWheel.suspensionAxis, nc::physics::ToVector3(actualSettings->mSuspensionDirection));
}

TEST_F(ConstraintFactoryTest, ModifyWheelAssembly_updatesDifferentials)
{
    auto body = CreateBody();
    SCOPE_EXIT(DestroyBody(body);)

    auto infoIn = nc::VehicleInfo{
        .wheelAssemblies = {
            nc::WheelAssembly{.differential = nc::Differential::MakeDisabled()},
            nc::WheelAssembly{},
        }
    };

    auto actualConstraint = uut.MakeVehicleConstraint(infoIn, *body);
    auto actualController = static_cast<JPH::WheeledVehicleController*>(actualConstraint->GetController());
    const auto& actualWheels = actualConstraint->GetWheels();
    const auto& actualDifferentials = actualController->GetDifferentials();

    ASSERT_EQ(4, actualWheels.size());
    ASSERT_EQ(1, actualDifferentials.size());
    ASSERT_EQ(2, actualDifferentials.at(0).mLeftWheel);
    ASSERT_EQ(3, actualDifferentials.at(0).mRightWheel);

    auto& target = infoIn.wheelAssemblies.at(0);

    // expect no change if already disabled
    nc::physics::ModifyWheelAssembly(target, *actualConstraint, *actualController);
    EXPECT_EQ(1, actualDifferentials.size());
    EXPECT_EQ(2, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(3, actualDifferentials.at(0).mRightWheel);
    EXPECT_FLOAT_EQ(1.0f, actualDifferentials.at(0).mEngineTorqueRatio);

    // expect enabling differential adds to constraint
    target.differential = nc::Differential{};
    nc::physics::ModifyWheelAssembly(target, *actualConstraint, *actualController);
    EXPECT_EQ(2, actualDifferentials.size());
    EXPECT_EQ(2, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(3, actualDifferentials.at(0).mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, actualDifferentials.at(0).mEngineTorqueRatio);
    EXPECT_EQ(0, actualDifferentials.at(1).mLeftWheel);
    EXPECT_EQ(1, actualDifferentials.at(1).mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, actualDifferentials.at(1).mEngineTorqueRatio);

    // expect already enabled differential gets rebuilt
    target.differential.ratio = 2.0f;
    nc::physics::ModifyWheelAssembly(target, *actualConstraint, *actualController);
    EXPECT_EQ(2, actualDifferentials.size());
    EXPECT_EQ(2, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(3, actualDifferentials.at(0).mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, actualDifferentials.at(0).mEngineTorqueRatio);
    EXPECT_EQ(0, actualDifferentials.at(1).mLeftWheel);
    EXPECT_EQ(1, actualDifferentials.at(1).mRightWheel);
    EXPECT_FLOAT_EQ(0.5f, actualDifferentials.at(1).mEngineTorqueRatio);
    EXPECT_FLOAT_EQ(2.0f, actualDifferentials.at(1).mDifferentialRatio);

    // expect disabling differential removes
    target.differential = nc::Differential::MakeDisabled();
    nc::physics::ModifyWheelAssembly(target, *actualConstraint, *actualController);
    EXPECT_EQ(1, actualDifferentials.size());
    EXPECT_EQ(2, actualDifferentials.at(0).mLeftWheel);
    EXPECT_EQ(3, actualDifferentials.at(0).mRightWheel);
    EXPECT_FLOAT_EQ(1.0f, actualDifferentials.at(0).mEngineTorqueRatio);
}
