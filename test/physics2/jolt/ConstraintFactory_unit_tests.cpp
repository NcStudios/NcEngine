#include "JoltApiFixture.inl"
#include "physics2/jolt/ConstraintFactory.h"

#include "physics2/jolt/Conversion.h"
#include "ncutility/ScopeExit.h"

#include "Jolt/Physics/Constraints/FixedConstraint.h"
#include "Jolt/Physics/Constraints/PointConstraint.h"

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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .axisX1 = nc::Vector3::Right(),
        .axisY1 = nc::Vector3::Up(),
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .axisX2 = nc::Vector3::Front(),
        .axisY2 = nc::Vector3::Down(),
        .detectFromPositions = false,
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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .axisX1 = nc::Vector3::Right(),
        .axisY1 = nc::Vector3::Up(),
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .axisX2 = nc::Vector3::Front(),
        .axisY2 = nc::Vector3::Down(),
        .detectFromPositions = false,
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
    // detectFromPosition is not saved to the settings
}

TEST_F(ConstraintFactoryTest, MakeConstraint_FixedConstraint_detectFromPositions_setsExpectedValues)
{
    const auto pos1 = JPH::Vec3{1.0f, 2.0f, 3.0f};
    const auto pos2 = JPH::Vec3{4.0f, 5.0f, 6.0f};

    auto body1 = CreateBody(pos1);
    auto body2 = CreateBody(pos2);
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::FixedConstraintInfo{
        .detectFromPositions = true,
        .space = nc::physics::ConstraintSpace::World
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::FixedConstraintSettings*>(baseSettings.GetPtr());

    // expected points are in local space
    constexpr auto scale = 0.5f;
    const auto expectedPoint1 = nc::physics::ToVector3((pos2 - pos1) * scale);
    const auto expectedPoint2 = nc::physics::ToVector3((pos1 - pos2) * scale);
    const auto expectedRight = nc::Vector3::Right();
    const auto expectedUp = nc::Vector3::Up();

    EXPECT_EQ(expectedPoint1, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(expectedRight, nc::physics::ToVector3(actualSettings->mAxisX1));
    EXPECT_EQ(expectedUp, nc::physics::ToVector3(actualSettings->mAxisY1));
    EXPECT_EQ(expectedPoint2, nc::physics::ToVector3(actualSettings->mPoint2));
    EXPECT_EQ(expectedRight, nc::physics::ToVector3(actualSettings->mAxisX2));
    EXPECT_EQ(expectedUp, nc::physics::ToVector3(actualSettings->mAxisY2));
    // detectFromPosition is not saved to the settings
}

TEST_F(ConstraintFactoryTest, MakeConstraint_FixedConstraint_detectFromPositionsWithLocalSpace_throws)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    SCOPE_EXIT
    (
        DestroyBody(body1);
        DestroyBody(body2);
    );

    const auto inConstraint = nc::physics::FixedConstraintInfo{
        .detectFromPositions = true,
        .space = nc::physics::ConstraintSpace::Local
    };

    EXPECT_THROW(uut.MakeConstraint(inConstraint, *body1, *body2), std::exception);
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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{-1.0f, -2.0f, -3.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *body2);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Point, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::PointConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.point1, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.point2, nc::physics::ToVector3(actualSettings->mPoint2));
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
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{-1.0f, -2.0f, -3.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    const auto baseConstraint = uut.MakeConstraint(inConstraint, *body1, *dummy);
    const auto actualType = baseConstraint->GetType();
    const auto actualSubType = baseConstraint->GetSubType();
    ASSERT_EQ(JPH::EConstraintType::TwoBodyConstraint, actualType);
    ASSERT_EQ(JPH::EConstraintSubType::Point, actualSubType);

    const auto baseSettings = baseConstraint->GetConstraintSettings();
    const auto actualSettings = static_cast<JPH::PointConstraintSettings*>(baseSettings.GetPtr());

    EXPECT_EQ(inConstraint.point1, nc::physics::ToVector3(actualSettings->mPoint1));
    EXPECT_EQ(inConstraint.point2, nc::physics::ToVector3(actualSettings->mPoint2));
}
