#include "gtest/gtest.h"
#include "ncengine/physics/PhysicsUtility.h"

TEST(PhysicsUtilityTest, BodyTypeConversion_roundTrip_preservesValue)
{
    constexpr auto dynamicType = nc::BodyType::Dynamic;
    constexpr auto kinematicType = nc::BodyType::Kinematic;
    constexpr auto staticType = nc::BodyType::Static;
    const auto dynamicName = nc::ToString(dynamicType);
    const auto kinematicName = nc::ToString(kinematicType);
    const auto staticName = nc::ToString(staticType);
    EXPECT_EQ(dynamicType, nc::ToBodyType(dynamicName));
    EXPECT_EQ(kinematicType, nc::ToBodyType(kinematicName));
    EXPECT_EQ(staticType, nc::ToBodyType(staticName));
}

TEST(PhysicsUtilityTest, BodyTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::BodyType>(255);
    EXPECT_THROW(nc::ToString(badType), std::exception);
    EXPECT_THROW(nc::ToBodyType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetBodyTypeNames_returnsValidNames)
{
    for (const auto& name : nc::GetBodyTypeNames())
    {
        EXPECT_NO_THROW(nc::ToBodyType(name));
    }
}

TEST(PhysicsUtilityTest, ShapeTypeConversion_roundTrip_preservesValues)
{
    constexpr auto boxType = nc::ShapeType::Box;
    constexpr auto sphereType = nc::ShapeType::Sphere;
    constexpr auto capsuleType = nc::ShapeType::Capsule;
    const auto boxName = nc::ToString(boxType);
    const auto sphereName = nc::ToString(sphereType);
    const auto capsuleName = nc::ToString(capsuleType);
    EXPECT_EQ(boxType, nc::ToShapeType(boxName));
    EXPECT_EQ(sphereType, nc::ToShapeType(sphereName));
    EXPECT_EQ(capsuleType, nc::ToShapeType(capsuleName));
}

TEST(PhysicsUtilityTest, ShapeTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::ShapeType>(255);
    EXPECT_THROW(nc::ToString(badType), std::exception);
    EXPECT_THROW(nc::ToShapeType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetShapeTypeNames_returnsValidNames)
{
    for (const auto& name : nc::GetShapeTypeNames())
    {
        EXPECT_NO_THROW(nc::ToShapeType(name));
    }
}

TEST(PhysicsUtilityTest, ConstraintTypeConversion_roundTrip_preservesValues)
{
    constexpr auto fixedConstraint = nc::ConstraintType::FixedConstraint;
    constexpr auto pointConstraint = nc::ConstraintType::PointConstraint;
    constexpr auto distanceConstraint = nc::ConstraintType::DistanceConstraint;
    constexpr auto hingeConstraint = nc::ConstraintType::HingeConstraint;
    constexpr auto sliderConstraint = nc::ConstraintType::SliderConstraint;
    constexpr auto swingTwistConstraint = nc::ConstraintType::SwingTwistConstraint;
    const auto fixedConstraintName = nc::ToString(fixedConstraint);
    const auto pointConstraintName = nc::ToString(pointConstraint);
    const auto distanceConstraintName = nc::ToString(distanceConstraint);
    const auto hingeConstraintName = nc::ToString(hingeConstraint);
    const auto sliderConstraintName = nc::ToString(sliderConstraint);
    const auto swingTwistConstraintName = nc::ToString(swingTwistConstraint);
    EXPECT_EQ(fixedConstraint, nc::ToConstraintType(fixedConstraintName));
    EXPECT_EQ(pointConstraint, nc::ToConstraintType(pointConstraintName));
    EXPECT_EQ(distanceConstraint, nc::ToConstraintType(distanceConstraintName));
    EXPECT_EQ(hingeConstraint, nc::ToConstraintType(hingeConstraintName));
    EXPECT_EQ(sliderConstraint, nc::ToConstraintType(sliderConstraintName));
    EXPECT_EQ(swingTwistConstraint, nc::ToConstraintType(swingTwistConstraintName));
}

TEST(PhysicsUtilityTest, ConstraintTypeConversion_badValue_throws)
{
    constexpr auto badType = static_cast<nc::ConstraintType>(255);
    EXPECT_THROW(nc::ToString(badType), std::exception);
    EXPECT_THROW(nc::ToConstraintType("invalid"), std::exception);
}

TEST(PhysicsUtilityTest, GetConstraintTypeNames_returnsValidNames)
{
    for (const auto& name : nc::GetConstraintTypeNames())
    {
        EXPECT_NO_THROW(nc::ToConstraintType(name));
    }
}
