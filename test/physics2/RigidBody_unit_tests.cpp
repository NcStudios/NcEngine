#include "gtest/gtest.h"
#include "ncengine/physics/RigidBody.h"

namespace nc::physics
{
class NcPhysicsImpl2
{
    public:
        void MockRegisterBody(RigidBody& body)
        {
            // Can use any values as long as we don't touch the interface (do that in the integration test)
            body.SetContext(static_cast<BodyHandle>(this), nullptr);
        }

        void MockUnregisterBody(RigidBody& body)
        {
            // Unregister so the destructor doesn't try to delete anything
            body.SetContext(nullptr, nullptr);
        }
};
} // namespace nc::physics

constexpr auto g_entity = nc::Entity{0, 0, nc::Entity::Flags::None};
constexpr auto g_staticEntity = nc::Entity{0, 0, nc::Entity::Flags::Static};
const auto g_shape = nc::physics::Shape::MakeBox();

constexpr auto g_dynamicInfo = nc::physics::RigidBodyInfo{
    .type = nc::physics::BodyType::Dynamic
};

constexpr auto g_staticInfo = nc::physics::RigidBodyInfo{
    .type = nc::physics::BodyType::Static
};

TEST(RigidBodyTests, IsInitialized_returnsCorrectState)
{
    auto uut = nc::physics::RigidBody{g_entity, g_shape, g_dynamicInfo};
    EXPECT_FALSE(uut.IsInitialized());
    auto mockModule = nc::physics::NcPhysicsImpl2{};
    mockModule.MockRegisterBody(uut);
    EXPECT_TRUE(uut.IsInitialized());
    mockModule.MockUnregisterBody(uut);
}

TEST(RigidBodyTests, Constructor_staticEntityWithStaticBody_setsToCorrectBodyType)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_staticInfo};
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
}

TEST(RigidBodyTests, Constructor_staticEntityWithDynamicBody_overwritesBodyType)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_dynamicInfo};
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
}

TEST(RigidBodyTests, MoveOperations_transferRegistrationData)
{
    auto first = nc::physics::RigidBody{g_entity, g_shape, g_dynamicInfo};
    auto mockModule = nc::physics::NcPhysicsImpl2{};
    mockModule.MockRegisterBody(first);
    auto second = nc::physics::RigidBody{std::move(first)};
    EXPECT_FALSE(first.GetEntity().Valid());
    EXPECT_FALSE(first.IsInitialized());
    EXPECT_TRUE(second.GetEntity().Valid());
    EXPECT_TRUE(second.IsInitialized());
    first = std::move(second);
    EXPECT_TRUE(first.GetEntity().Valid());
    EXPECT_TRUE(first.IsInitialized());
    EXPECT_FALSE(second.GetEntity().Valid());
    EXPECT_FALSE(second.IsInitialized());
    mockModule.MockUnregisterBody(first);
}

TEST(RigidBodyTests, TrivialGetters_returnExpectedValues)
{
    const auto uut = nc::physics::RigidBody{g_entity, g_shape, g_dynamicInfo};
    EXPECT_EQ(g_entity, uut.GetEntity());
    EXPECT_EQ(g_shape.GetType(), uut.GetShape().GetType());
    EXPECT_EQ(g_dynamicInfo.type, uut.GetBodyType());
    EXPECT_FLOAT_EQ(g_dynamicInfo.friction, uut.GetFriction());
    EXPECT_FLOAT_EQ(g_dynamicInfo.restitution, uut.GetRestitution());
    EXPECT_FLOAT_EQ(g_dynamicInfo.linearDamping, uut.GetLinearDamping());
    EXPECT_FLOAT_EQ(g_dynamicInfo.angularDamping, uut.GetAngularDamping());
    EXPECT_FLOAT_EQ(g_dynamicInfo.gravityMultiplier, uut.GetGravityMultiplier());
    EXPECT_TRUE(uut.ScalesWithTransform());
    EXPECT_FALSE(uut.UseContinuousDetection());
}
