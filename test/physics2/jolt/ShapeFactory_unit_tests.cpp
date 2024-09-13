#include "gtest/gtest.h"
#include "JobSystem_stub.inl"
#include "physics2/jolt/ShapeFactory.h"
#include "physics2/jolt/JoltApi.h"
#include "ncengine/config/Config.h"

class ShapeFactoryTest : public ::testing::Test
{
    private:
        nc::physics::JoltApi m_jolt;

    protected:
        ShapeFactoryTest()
            : m_jolt{nc::physics::JoltApi::Initialize(
                  nc::config::MemorySettings{},
                  nc::config::PhysicsSettings{
                    .tempAllocatorSize = 1024 * 1024 * 4,
                    .maxBodyPairs = 8,
                    .maxContacts = 4
                  },
                  nc::task::AsyncDispatcher{}
              )}
        {
        }

    public:
        nc::physics::ShapeFactory uut;
};

TEST_F(ShapeFactoryTest, MakeShape_box_returnsBoxShape)
{
    const auto inShape = nc::physics::Shape::MakeBox(nc::Vector3{1.0f, 2.0f, 3.0f}, nc::Vector3::Zero());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());
    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, innerShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(innerShape);

    const auto expectedScale = inShape.GetLocalScale();
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);

    const auto expectedPosition = inShape.GetLocalPosition();
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);
}

TEST_F(ShapeFactoryTest, MakeShape_box_withTransformScaling_returnsBoxShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::physics::Shape::MakeBox(nc::Vector3{2.0f, 2.0f, 2.0f}, nc::Vector3{0.0f, 1.0f, 0.0f});
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = nc::HadamardProduct(inShape.GetLocalPosition(), transformScale);
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Box, innerShape->GetSubType());
    const auto box = static_cast<const JPH::BoxShape*>(innerShape);

    const auto expectedScale = nc::HadamardProduct(inShape.GetLocalScale(), transformScale);
    const auto actualScale = nc::physics::ToVector3(box->GetHalfExtent() * 2.0f);
    EXPECT_EQ(expectedScale, actualScale);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_returnsBoxShape)
{
    const auto inShape = nc::physics::Shape::MakeSphere(0.75, nc::Vector3::Zero());
    const auto wrappedShape = uut.MakeShape(inShape, JPH::Vec3::sReplicate(1.0f));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = inShape.GetLocalPosition();
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, innerShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(innerShape);

    const auto expectedRadius = inShape.GetLocalScale().x * 0.5f;
    const auto actualRadius = sphere->GetRadius();
    EXPECT_FLOAT_EQ(expectedRadius, actualRadius);
}

TEST_F(ShapeFactoryTest, MakeShape_sphere_withTransformScaling_returnsBoxShape)
{
    const auto transformScale = nc::Vector3{2.0f, 2.0f, 2.0f};
    const auto inShape = nc::physics::Shape::MakeSphere(0.75, nc::Vector3{1.0f, 2.0f, 3.0f});
    const auto wrappedShape = uut.MakeShape(inShape, nc::physics::ToJoltVec3(transformScale));

    ASSERT_EQ(JPH::EShapeType::Decorated, wrappedShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::RotatedTranslated, wrappedShape->GetSubType());
    const auto decoratedShape = static_cast<const JPH::RotatedTranslatedShape*>(wrappedShape.GetPtr());

    const auto expectedPosition = nc::HadamardProduct(inShape.GetLocalPosition(), transformScale);
    const auto actualPosition = nc::physics::ToVector3(decoratedShape->GetPosition());
    EXPECT_EQ(expectedPosition, actualPosition);

    const auto innerShape = decoratedShape->GetInnerShape();
    ASSERT_EQ(JPH::EShapeType::Convex, innerShape->GetType());
    ASSERT_EQ(JPH::EShapeSubType::Sphere, innerShape->GetSubType());
    const auto sphere = static_cast<const JPH::SphereShape*>(innerShape);

    const auto expectedRadius = inShape.GetLocalScale().x * transformScale.x * 0.5f;
    const auto actualRadius = sphere->GetRadius();
    EXPECT_FLOAT_EQ(expectedRadius, actualRadius);
}
