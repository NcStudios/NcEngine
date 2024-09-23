#include "JoltApiFixture.inl"
#include "physics2/jolt/BodyFactory.h"
#include "physics2/jolt/ShapeFactory.h"
#include "ncengine/physics/RigidBody.h"

class BodyFactoryTest : public JoltApiFixture
{
    protected:
        nc::physics::ShapeFactory shapeFactory;
        nc::physics::BodyFactory uut;

        BodyFactoryTest()
            : uut{joltApi.physicsSystem.GetBodyInterfaceNoLock(), shapeFactory}
        {
        }

        void DestroyBody(const JPH::Body* body)
        {
            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            const auto id = body->GetID();
            interface.DestroyBody(id); // note: factory doesn't add body, so don't call remove
        }
};

constexpr auto g_entity = nc::Entity{42, 0, 0};
constexpr auto g_dynamicProperties = nc::physics::RigidBodyInfo{
    .friction = 1.0f,
    .restitution = 0.9f,
    .linearDamping = 0.5f,
    .angularDamping = 0.4f,
    .gravityMultiplier = 0.1f,
    .type = nc::physics::BodyType::Dynamic,
    .freedom = nc::physics::DegreeOfFreedom::TranslationY,
    .flags = nc::physics::RigidBodyFlags::ContinuousDetection
};

TEST_F(BodyFactoryTest, MakeBody_setsBodyProperties)
{
    const auto box = nc::physics::Shape::MakeBox();
    const auto& expectedProperties = g_dynamicProperties;
    const auto rigidBody = nc::physics::RigidBody{g_entity, box, expectedProperties};
    const auto matrix = DirectX::XMMatrixIdentity();

    const auto result = uut.MakeBody(rigidBody, matrix);
    EXPECT_FALSE(result.wasScaleAdjusted);
    const auto actualBody = result.body;

    EXPECT_EQ(nc::Vector3::Zero(), nc::physics::ToVector3(actualBody->GetPosition()));
    EXPECT_EQ(nc::Quaternion::Identity(), nc::physics::ToQuaternion(actualBody->GetRotation()));
    // skip detailed shape/scale checks - shape wrapping is implementation detail of ShapeFactory and subject to change; covered by ShapeFactory tests
    EXPECT_FLOAT_EQ(1.0f, actualBody->GetShape()->GetVolume());

    EXPECT_EQ(JPH::EMotionType::Dynamic, actualBody->GetMotionType());
    EXPECT_EQ(nc::physics::ObjectLayer::Dynamic, actualBody->GetObjectLayer());
    EXPECT_FALSE(actualBody->IsSensor());
    EXPECT_EQ(nc::Entity::Hash{}(g_entity), actualBody->GetUserData());
    EXPECT_EQ(expectedProperties.friction, actualBody->GetFriction());
    EXPECT_EQ(expectedProperties.restitution, actualBody->GetRestitution());

    const auto& actualMotionProperties = actualBody->GetMotionProperties();
    EXPECT_EQ(JPH::EAllowedDOFs::TranslationY, actualMotionProperties->GetAllowedDOFs());
    EXPECT_EQ(JPH::EMotionQuality::LinearCast, actualMotionProperties->GetMotionQuality());
    EXPECT_EQ(expectedProperties.linearDamping, actualMotionProperties->GetLinearDamping());
    EXPECT_EQ(expectedProperties.angularDamping, actualMotionProperties->GetAngularDamping());
    EXPECT_EQ(expectedProperties.gravityMultiplier, actualMotionProperties->GetGravityFactor());

    DestroyBody(actualBody);
}

TEST_F(BodyFactoryTest, MakeBody_invalidTransformScale_returnsAdjustedValue)
{
    const auto sphere = nc::physics::Shape::MakeSphere();
    const auto rigidBody = nc::physics::RigidBody{g_entity, sphere, g_dynamicProperties};
    const auto matrix = DirectX::XMMatrixScaling(1.0f, 2.0f, 3.0f);
    const auto result = uut.MakeBody(rigidBody, matrix);
    EXPECT_TRUE(result.wasScaleAdjusted);
    EXPECT_EQ(nc::Vector3::Splat(2.0f), result.adjustedScale);

    DestroyBody(result.body);
}
