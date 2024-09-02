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
constexpr auto g_shape = nc::physics::Shape::Box;
constexpr auto g_dynamicBodyType = nc::physics::BodyType::Dynamic;
constexpr auto g_staticBodyType = nc::physics::BodyType::Static;
constexpr auto g_flags = nc::physics::RigidBodyFlags::ScaleWithTransform;

TEST(RigidBodyTests, IsInitialized_returnsCorrectState)
{
    auto uut = nc::physics::RigidBody{g_entity, g_shape, g_dynamicBodyType, g_flags};
    EXPECT_FALSE(uut.IsInitialized());
    auto mockModule = nc::physics::NcPhysicsImpl2{};
    mockModule.MockRegisterBody(uut);
    EXPECT_TRUE(uut.IsInitialized());
    mockModule.MockUnregisterBody(uut);
}

TEST(RigidBodyTests, SetContext_staticEntityWithStaticBody_succeeds)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_staticBodyType};
    auto mockModule = nc::physics::NcPhysicsImpl2{};
    EXPECT_NO_THROW(mockModule.MockRegisterBody(uut));
    mockModule.MockUnregisterBody(uut);
}

TEST(RigidBodyTests, SetContext_staticEntityWithDynamicBody_throws)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_dynamicBodyType};
    auto mockModule = nc::physics::NcPhysicsImpl2{};
    EXPECT_THROW(mockModule.MockRegisterBody(uut), nc::NcError);
}

TEST(RigidBodyTests, MoveOperations_transferRegistrationData)
{
    auto first = nc::physics::RigidBody{g_entity, g_shape, g_dynamicBodyType};
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
    const auto uut = nc::physics::RigidBody{g_entity, g_shape, g_dynamicBodyType, g_flags};
    EXPECT_EQ(g_entity, uut.GetEntity());
    EXPECT_EQ(g_shape, uut.GetShape());
    EXPECT_EQ(g_dynamicBodyType, uut.GetBodyType());
    EXPECT_TRUE(uut.ScalesWithTransform());
}
