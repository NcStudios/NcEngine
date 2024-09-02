#include "gtest/gtest.h"
#include "physics2/jolt/ShapeFactory.h"
#include "physics2/jolt/JoltApi.h"

class ShapeFactoryTest : public ::testing::Test
{
    private:
        nc::physics::JoltApi m_jolt;

    protected:
        ShapeFactoryTest()
            : m_jolt{nc::physics::JoltApi::Initialize()}
        {
        }

    public:
        nc::physics::ShapeFactory uut;
};

TEST(ShapeFactoryUtilityTest, NormalizeScaleForShape_box_doesNotModify)
{
    constexpr auto shape = nc::physics::Shape::Box;
    const auto expectedScale1 = JPH::Vec3{1.0f, 1.0f, 1.0f};
    const auto expectedScale2 = JPH::Vec3{1.0f, 2.0f, 3.0f};
    auto actualScale1 = expectedScale1;
    auto actualScale2 = expectedScale2;
    EXPECT_FALSE(nc::physics::NormalizeScaleForShape(shape, actualScale1));
    EXPECT_FALSE(nc::physics::NormalizeScaleForShape(shape, actualScale2));
    EXPECT_EQ(expectedScale1, actualScale1);
    EXPECT_EQ(expectedScale2, actualScale2);
}

TEST(ShapeFactoryUtilityTest, NormalizeScaleForShape_sphere_uniformScaling_doesNotModify)
{
    constexpr auto shape = nc::physics::Shape::Sphere;
    const auto expectedScale = JPH::Vec3{3.0f, 3.0f, 3.0f};
    auto actualScale = expectedScale;
    EXPECT_FALSE(nc::physics::NormalizeScaleForShape(shape, actualScale));
    EXPECT_EQ(expectedScale, actualScale);
}

TEST(ShapeFactoryUtilityTest, NormalizeScaleForShape_sphere_nonUniformScaling_fixesScale)
{
    constexpr auto shape = nc::physics::Shape::Sphere;
    auto actualScale = JPH::Vec3{1.0f, 2.0f, 3.0f};
    const auto averageScale = (actualScale.GetX() + actualScale.GetY() + actualScale.GetZ()) / 3.0f;
    const auto expectedScale = JPH::Vec3::sReplicate(averageScale);
    EXPECT_TRUE(nc::physics::NormalizeScaleForShape(shape, actualScale));
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_box_returnsBoxShape)
{
    const auto expectedScale = JPH::Vec3{1.0f, 2.0f, 3.0f};
    const auto shape = uut.MakeShape(nc::physics::Shape::Box, expectedScale);

    ASSERT_EQ(JPH::EShapeType::Convex, shape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, shape->GetSubType());

    const auto box = static_cast<const JPH::BoxShape*>(shape.GetPtr());
    const auto actualScale = box->GetHalfExtent() * 2.0f;

    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_returnsBoxShape)
{
    const auto expectedRadius = 0.75f;
    const auto shape = uut.MakeShape(nc::physics::Shape::Sphere, JPH::Vec3::sReplicate(expectedRadius * 2.0f));

    ASSERT_EQ(JPH::EShapeType::Convex, shape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, shape->GetSubType());

    const auto sphere = static_cast<const JPH::SphereShape*>(shape.GetPtr());
    EXPECT_FLOAT_EQ(expectedRadius, sphere->GetRadius());
}
