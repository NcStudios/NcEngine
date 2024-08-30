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
};

TEST_F(ShapeFactoryTest, MakeShape_box_returnsBoxShape)
{
    const auto expectedScale = JPH::Vec3{1.0f, 2.0f, 3.0f};
    const auto shape = nc::physics::MakeShape(nc::physics::Shape::Box, expectedScale);

    ASSERT_EQ(JPH::EShapeType::Convex, shape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, shape->GetSubType());

    const auto box = static_cast<const JPH::BoxShape*>(shape.GetPtr());
    const auto actualScale = box->GetHalfExtent() * 2.0f;

    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_returnsBoxShape)
{
    const auto expectedRadius = 0.75f;
    const auto shape = nc::physics::MakeShape(nc::physics::Shape::Sphere, JPH::Vec3::sReplicate(expectedRadius * 2.0f));

    ASSERT_EQ(JPH::EShapeType::Convex, shape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, shape->GetSubType());

    const auto sphere = static_cast<const JPH::SphereShape*>(shape.GetPtr());
    EXPECT_FLOAT_EQ(expectedRadius, sphere->GetRadius());
}
