#include "JoltApiFixture.inl"
#include "physics2/jolt/ConstraintFactory.h"

#include "physics2/jolt/Conversion.h"
#include "ncutility/ScopeExit.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Constraints/FixedConstraint.h"

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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .axisX1 = nc::Vector3::Right(),
        .axisY1 = nc::Vector3::Up(),
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .axisX2 = nc::Vector3::Front(),
        .axisY2 = nc::Vector3::Down(),
        .space = nc::physics::ConstraintSpace::World
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Fixed, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::FixedConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.point1, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.axisX1, nc::physics::ToVector3(actualSettings->mAxisX1));
    EXPECT_EQ(inConstraint.axisY1, nc::physics::ToVector3(actualSettings->mAxisY1));
    EXPECT_EQ(inConstraint.point2, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.axisX2, nc::physics::ToVector3(actualSettings->mAxisX2));
    EXPECT_EQ(inConstraint.axisY2, nc::physics::ToVector3(actualSettings->mAxisY2));
    EXPECT_EQ(inConstraint.autoDetect, actualSettings->mAutoDetectPoint);
    // can't test ConstraintSpace, returned settings are always local
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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .axisX1 = nc::Vector3::Right(),
        .axisY1 = nc::Vector3::Up(),
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .axisX2 = nc::Vector3::Front(),
        .axisY2 = nc::Vector3::Down(),
        .space = nc::physics::ConstraintSpace::Local
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *dummy);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Fixed, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::FixedConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.point1, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.axisX1, nc::physics::ToVector3(actualSettings->mAxisX1));
    EXPECT_EQ(inConstraint.axisY1, nc::physics::ToVector3(actualSettings->mAxisY1));
    EXPECT_EQ(inConstraint.point2, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(inConstraint.axisX2, nc::physics::ToVector3(actualSettings->mAxisX2));
    EXPECT_EQ(inConstraint.axisY2, nc::physics::ToVector3(actualSettings->mAxisY2));
    EXPECT_EQ(inConstraint.autoDetect, actualSettings->mAutoDetectPoint);
    // can't test ConstraintSpace, returned settings are always local
}
