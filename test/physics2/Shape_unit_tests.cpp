#include "gtest/gtest.h"
#include "ncengine/physics/Shape.h"

TEST(ShapeTest, NormalizeScaleForShape_box_doesNotModify)
{
    constexpr auto shape = nc::physics::ShapeType::Box;
    const auto expectedScale1 = nc::Vector3{1.0f, 1.0f, 1.0f};
    const auto expectedScale2 = nc::Vector3{1.0f, 2.0f, 3.0f};
    auto actualScale1 = nc::physics::NormalizeScaleForShape(shape, expectedScale2, expectedScale1);
    auto actualScale2 = nc::physics::NormalizeScaleForShape(shape, expectedScale1, expectedScale2);
    EXPECT_EQ(expectedScale1, actualScale1);
    EXPECT_EQ(expectedScale2, actualScale2);
}

TEST(ShapeTest, NormalizeScaleForShape_box_zeroScale_setsToMinimumScale)
{
    constexpr auto shape = nc::physics::ShapeType::Box;
    const auto badScale = nc::Vector3{0.0f, 1.0f, 1.0f};
    const auto expectedScale = nc::Vector3{nc::physics::g_minimumShapeScale, 1.0f, 1.0f};
    auto actualScale = nc::physics::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_uniformScaling_doesNotModify)
{
    constexpr auto shape = nc::physics::ShapeType::Sphere;
    const auto expectedScale = nc::Vector3{3.0f, 3.0f, 3.0f};
    auto actualScale = nc::physics::NormalizeScaleForShape(shape, nc::Vector3::One(), expectedScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_nonUniformScaling_fixesScale)
{
    constexpr auto shape = nc::physics::ShapeType::Sphere;
    const auto initialScale = nc::Vector3::One();
    const auto attemptScaleX = nc::Vector3{3.0f, 1.0f, 1.0f};
    const auto attemptScaleY = nc::Vector3{1.0f, 0.5f, 1.0f};
    const auto attemptScaleZ = nc::Vector3{1.0f, 1.0f, 10.0f};
    const auto expectedUniformFromX = nc::Vector3::Splat(3.0f);
    const auto expectedUniformFromY = nc::Vector3::Splat(0.5f);
    const auto expectedUniformFromZ = nc::Vector3::Splat(10.0f);
    const auto actualUniformFromX = nc::physics::NormalizeScaleForShape(shape, initialScale, attemptScaleX);
    const auto actualUniformFromY = nc::physics::NormalizeScaleForShape(shape, initialScale, attemptScaleY);
    const auto actualUniformFromZ = nc::physics::NormalizeScaleForShape(shape, initialScale, attemptScaleZ);
    EXPECT_EQ(expectedUniformFromX, actualUniformFromX);
    EXPECT_EQ(expectedUniformFromY, actualUniformFromY);
    EXPECT_EQ(expectedUniformFromZ, actualUniformFromZ);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_zeroScale_fixesScale)
{
    constexpr auto shape = nc::physics::ShapeType::Sphere;
    const auto badScale = nc::Vector3{1.0f, 0.0f, 1.0f};
    const auto expectedScale = nc::Vector3::Splat(nc::physics::g_minimumShapeScale);
    auto actualScale = nc::physics::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_uniformXZScaling_doesNotModify)
{
    constexpr auto shape = nc::physics::ShapeType::Capsule;
    const auto initialScale = nc::Vector3{0.5f, 2.0f, 0.5f};
    const auto expectedScale = nc::Vector3{1.0f, 2.0f, 1.0f};
    auto actualScale = nc::physics::NormalizeScaleForShape(shape, initialScale, expectedScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_nonUniformXZScaling_fixesScale)
{
    constexpr auto shape = nc::physics::ShapeType::Capsule;
    const auto initialScale = nc::Vector3{1.0f, 2.0f, 1.0f};
    const auto attemptScaleX = nc::Vector3{3.0f, 2.0f, 1.0f};
    const auto attemptScaleZ = nc::Vector3{1.0f, 2.0f, 0.5f};
    const auto expectedUniformFromX = nc::Vector3{3.0f, 2.0f, 3.0f};
    const auto expectedUniformFromZ = nc::Vector3{0.5f, 2.0f, 0.5f};
    const auto actualUniformFromX = nc::physics::NormalizeScaleForShape(shape, initialScale, attemptScaleX);
    const auto actualUniformFromZ = nc::physics::NormalizeScaleForShape(shape, initialScale, attemptScaleZ);
    EXPECT_EQ(expectedUniformFromX, actualUniformFromX);
    EXPECT_EQ(expectedUniformFromZ, actualUniformFromZ);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_zeroScale_fixesScale)
{
    constexpr auto shape = nc::physics::ShapeType::Capsule;
    const auto badScale = nc::Vector3{1.0f, 1.0f, 0.0f};
    const auto expectedScale = nc::Vector3{nc::physics::g_minimumShapeScale, 1.0f, nc::physics::g_minimumShapeScale};
    auto actualScale = nc::physics::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}