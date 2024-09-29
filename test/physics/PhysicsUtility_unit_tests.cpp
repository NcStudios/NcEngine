#include "gtest/gtest.h"
#include "ncengine/physics/PhysicsUtility.h"

TEST(PhysicsUtilityTest, BodyTypeConversion_roundTrip_preservesValue)
{
    constexpr auto dynamicType = nc::physics::BodyType::Dynamic;
    constexpr auto kinematicType = nc::physics::BodyType::Kinematic;
    constexpr auto staticType = nc::physics::BodyType::Static;
    const auto dynamicName = nc::physics::ToString(dynamicType);
    const auto kinematicName = nc::physics::ToString(kinematicType);
    const auto staticName = nc::physics::ToString(staticType);
    EXPECT_EQ(dynamicType, nc::physics::ToBodyType(dynamicName));
    EXPECT_EQ(kinematicType, nc::physics::ToBodyType(kinematicName));
    EXPECT_EQ(staticType, nc::physics::ToBodyType(staticName));
}

TEST(PhysicsUtilityTest, BodyTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::physics::BodyType>(255);
    EXPECT_THROW(nc::physics::ToString(badType), std::exception);
    EXPECT_THROW(nc::physics::ToBodyType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetBodyTypeNames_returnsValidNames)
{
    for (const auto& name : nc::physics::GetBodyTypeNames())
    {
        EXPECT_NO_THROW(nc::physics::ToBodyType(name));
    }
}

TEST(PhysicsUtilityTest, ShapeTypeConversion_roundTrip_preservesValues)
{
    constexpr auto boxType = nc::physics::ShapeType::Box;
    constexpr auto sphereType = nc::physics::ShapeType::Sphere;
    constexpr auto capsuleType = nc::physics::ShapeType::Capsule;
    const auto boxName = nc::physics::ToString(boxType);
    const auto sphereName = nc::physics::ToString(sphereType);
    const auto capsuleName = nc::physics::ToString(capsuleType);
    EXPECT_EQ(boxType, nc::physics::ToShapeType(boxName));
    EXPECT_EQ(sphereType, nc::physics::ToShapeType(sphereName));
    EXPECT_EQ(capsuleType, nc::physics::ToShapeType(capsuleName));
}

TEST(PhysicsUtilityTest, ShapeTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::physics::ShapeType>(255);
    EXPECT_THROW(nc::physics::ToString(badType), std::exception);
    EXPECT_THROW(nc::physics::ToShapeType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetShapeTypeNames_returnsValidNames)
{
    for (const auto& name : nc::physics::GetShapeTypeNames())
    {
        EXPECT_NO_THROW(nc::physics::ToShapeType(name));
    }
}

TEST(PhysicsUtilityTest, ConstraintTypeConversion_roundTrip_preservesValues)
{
    constexpr auto fixedConstraint = nc::physics::ConstraintType::FixedConstraint;
    constexpr auto pointConstraint = nc::physics::ConstraintType::PointConstraint;
    constexpr auto distanceConstraint = nc::physics::ConstraintType::DistanceConstraint;
    constexpr auto hingeConstraint = nc::physics::ConstraintType::HingeConstraint;
    constexpr auto sliderConstraint = nc::physics::ConstraintType::SliderConstraint;
    constexpr auto swingTwistConstraint = nc::physics::ConstraintType::SwingTwistConstraint;
    const auto fixedConstraintName = nc::physics::ToString(fixedConstraint);
    const auto pointConstraintName = nc::physics::ToString(pointConstraint);
    const auto distanceConstraintName = nc::physics::ToString(distanceConstraint);
    const auto hingeConstraintName = nc::physics::ToString(hingeConstraint);
    const auto sliderConstraintName = nc::physics::ToString(sliderConstraint);
    const auto swingTwistConstraintName = nc::physics::ToString(swingTwistConstraint);
    EXPECT_EQ(fixedConstraint, nc::physics::ToConstraintType(fixedConstraintName));
    EXPECT_EQ(pointConstraint, nc::physics::ToConstraintType(pointConstraintName));
    EXPECT_EQ(distanceConstraint, nc::physics::ToConstraintType(distanceConstraintName));
    EXPECT_EQ(hingeConstraint, nc::physics::ToConstraintType(hingeConstraintName));
    EXPECT_EQ(sliderConstraint, nc::physics::ToConstraintType(sliderConstraintName));
    EXPECT_EQ(swingTwistConstraint, nc::physics::ToConstraintType(swingTwistConstraintName));
}

TEST(PhysicsUtilityTest, ConstraintTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::physics::ConstraintType>(255);
    EXPECT_THROW(nc::physics::ToString(badType), std::exception);
    EXPECT_THROW(nc::physics::ToConstraintType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetConstraintTypeNames_returnsValidNames)
{
    for (const auto& name : nc::physics::GetConstraintTypeNames())
    {
        EXPECT_NO_THROW(nc::physics::ToConstraintType(name));
    }
}
