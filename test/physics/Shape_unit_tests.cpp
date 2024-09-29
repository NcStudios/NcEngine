#include "gtest/gtest.h"
#include "ncengine/physics/Shape.h"

TEST(ShapeTest, NormalizeScaleForShape_box_doesNotModify)
{
    constexpr auto shape = nc::ShapeType::Box;
    const auto expectedScale1 = nc::Vector3{1.0f, 1.0f, 1.0f};
    const auto expectedScale2 = nc::Vector3{1.0f, 2.0f, 3.0f};
    auto actualScale1 = nc::NormalizeScaleForShape(shape, expectedScale2, expectedScale1);
    auto actualScale2 = nc::NormalizeScaleForShape(shape, expectedScale1, expectedScale2);
    EXPECT_EQ(expectedScale1, actualScale1);
    EXPECT_EQ(expectedScale2, actualScale2);
}

TEST(ShapeTest, NormalizeScaleForShape_box_zeroScale_setsToMinimumScale)
{
    constexpr auto shape = nc::ShapeType::Box;
    const auto badScale = nc::Vector3{0.0f, 1.0f, 1.0f};
    const auto expectedScale = nc::Vector3{nc::g_minShapeScale, 1.0f, 1.0f};
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_box_scaleTooLarge_setsToMaximum)
{
    constexpr auto shape = nc::ShapeType::Box;
    const auto badScale = nc::Vector3{1.0f, 1.0f, nc::g_maxShapeScale + 1.0f};
    const auto expectedScale = nc::Vector3{1.0f, 1.0f, nc::g_maxShapeScale};
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_uniformScaling_doesNotModify)
{
    constexpr auto shape = nc::ShapeType::Sphere;
    const auto expectedScale = nc::Vector3{3.0f, 3.0f, 3.0f};
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), expectedScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_nonUniformScaling_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Sphere;
    const auto initialScale = nc::Vector3::One();
    const auto attemptScaleX = nc::Vector3{3.0f, 1.0f, 1.0f};
    const auto attemptScaleY = nc::Vector3{1.0f, 0.5f, 1.0f};
    const auto attemptScaleZ = nc::Vector3{1.0f, 1.0f, 10.0f};
    const auto expectedUniformFromX = nc::Vector3::Splat(3.0f);
    const auto expectedUniformFromY = nc::Vector3::Splat(0.5f);
    const auto expectedUniformFromZ = nc::Vector3::Splat(10.0f);
    const auto actualUniformFromX = nc::NormalizeScaleForShape(shape, initialScale, attemptScaleX);
    const auto actualUniformFromY = nc::NormalizeScaleForShape(shape, initialScale, attemptScaleY);
    const auto actualUniformFromZ = nc::NormalizeScaleForShape(shape, initialScale, attemptScaleZ);
    EXPECT_EQ(expectedUniformFromX, actualUniformFromX);
    EXPECT_EQ(expectedUniformFromY, actualUniformFromY);
    EXPECT_EQ(expectedUniformFromZ, actualUniformFromZ);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_nonUniformScaling_sameCurrentAndDesiredScale_averagesScale)
{
    constexpr auto shape = nc::ShapeType::Sphere;
    const auto nonUniformScale = nc::Vector3{1.0, 2.0f, 3.0f};
    const auto actualScale = nc::NormalizeScaleForShape(shape, nonUniformScale, nonUniformScale);
    const auto expectedScale = nc::Vector3::Splat(2.0f);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_zeroScale_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Sphere;
    const auto badScale = nc::Vector3{1.0f, 0.0f, 1.0f};
    const auto expectedScale = nc::Vector3::Splat(nc::g_minShapeScale);
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_sphere_scaleTooLarge_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Sphere;
    const auto badScale = nc::Vector3::Splat(nc::g_maxShapeScale + 20.0f);
    const auto expectedScale = nc::Vector3::Splat(nc::g_maxShapeScale);
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_uniformXZScaling_doesNotModify)
{
    constexpr auto shape = nc::ShapeType::Capsule;
    const auto initialScale = nc::Vector3{0.5f, 2.0f, 0.5f};
    const auto expectedScale = nc::Vector3{1.0f, 2.0f, 1.0f};
    auto actualScale = nc::NormalizeScaleForShape(shape, initialScale, expectedScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_nonUniformXZScaling_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Capsule;
    const auto initialScale = nc::Vector3{1.0f, 2.0f, 1.0f};
    const auto attemptScaleX = nc::Vector3{3.0f, 2.0f, 1.0f};
    const auto attemptScaleZ = nc::Vector3{1.0f, 2.0f, 0.5f};
    const auto expectedUniformFromX = nc::Vector3{3.0f, 2.0f, 3.0f};
    const auto expectedUniformFromZ = nc::Vector3{0.5f, 2.0f, 0.5f};
    const auto actualUniformFromX = nc::NormalizeScaleForShape(shape, initialScale, attemptScaleX);
    const auto actualUniformFromZ = nc::NormalizeScaleForShape(shape, initialScale, attemptScaleZ);
    EXPECT_EQ(expectedUniformFromX, actualUniformFromX);
    EXPECT_EQ(expectedUniformFromZ, actualUniformFromZ);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_nonUniformScaling_sameCurrentAndDesiredScale_averagesScale)
{
    constexpr auto shape = nc::ShapeType::Capsule;
    const auto nonUniformScale = nc::Vector3{2.0, 2.0f, 3.0f};
    const auto actualScale = nc::NormalizeScaleForShape(shape, nonUniformScale, nonUniformScale);
    const auto expectedScale = nc::Vector3{2.5f, 2.0f, 2.5f};
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_zeroScale_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Capsule;
    const auto badScale = nc::Vector3{1.0f, 1.0f, 0.0f};
    const auto expectedScale = nc::Vector3{nc::g_minShapeScale, 1.0f, nc::g_minShapeScale};
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeTest, NormalizeScaleForShape_capsule_scaleTooLarge_fixesScale)
{
    constexpr auto shape = nc::ShapeType::Capsule;
    const auto badScale = nc::Vector3{nc::g_maxShapeScale + 1.0f, 1.0f, 1.0f};
    const auto expectedScale = nc::Vector3{nc::g_maxShapeScale, 1.0f, nc::g_maxShapeScale};
    auto actualScale = nc::NormalizeScaleForShape(shape, nc::Vector3::One(), badScale);
    EXPECT_EQ(expectedScale, actualScale);
}
