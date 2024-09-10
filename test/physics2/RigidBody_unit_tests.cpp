#include "gtest/gtest.h"
#include "ncengine/config/Config.h"
#include "ncengine/physics/RigidBody.h"
#include "physics2/jolt/JoltApi.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"

#include <vector>

namespace nc
{
namespace physics
{
class NcPhysicsImpl2
{
    public:
        void MockRegisterBody(RigidBody& body, JPH::Body* apiBody, ComponentContext& ctx)
        {
            body.SetContext(static_cast<BodyHandle>(apiBody), &ctx);
        }
};
} // namespace physics

namespace task
{
class AsyncDispatcher{};
} // namespace task
} // namespace nc

class RigidBodyTest : public ::testing::Test
{
    protected:
        RigidBodyTest()
            : joltApi{nc::physics::JoltApi::Initialize(
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
        nc::physics::JoltApi joltApi;
        nc::physics::NcPhysicsImpl2 ncPhysics;
        std::vector<JPH::Body*> bodies;

        auto CreateRigidBody(nc::Entity entity,
                             const nc::physics::Shape& shape,
                             const nc::physics::RigidBodyInfo& info,
                             const JPH::Vec3& position = JPH::Vec3::sZero(),
                             const JPH::Vec3& scale = JPH::Vec3::sReplicate(1.0f))
        {
            auto body = nc::physics::RigidBody{entity, shape, info};
            auto settings = JPH::BodyCreationSettings{
                joltApi.shapeFactory.MakeShape(shape, scale),
                position,
                JPH::Quat::sIdentity(),
                nc::physics::ToMotionType(info.type),
                nc::physics::ToObjectLayer(info.type)
            };

            settings.mUserData = nc::Entity::Hash{}(entity);
            settings.mMotionQuality = nc::physics::ToMotionQuality(body.UseContinuousDetection());
            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            auto apiBody = interface.CreateBody(settings);
            interface.AddBody(apiBody->GetID(), JPH::EActivation::Activate);
            ncPhysics.MockRegisterBody(body, apiBody, *joltApi.ctx);
            bodies.push_back(apiBody);
            return body;
        }
};

constexpr auto g_entity = nc::Entity{0, 0, nc::Entity::Flags::None};
constexpr auto g_staticEntity = nc::Entity{0, 0, nc::Entity::Flags::Static};
constexpr auto g_shape = nc::physics::Shape::MakeBox();

constexpr auto g_dynamicInfo = nc::physics::RigidBodyInfo{
    .type = nc::physics::BodyType::Dynamic
};

constexpr auto g_kinematicInfo = nc::physics::RigidBodyInfo{
    .type = nc::physics::BodyType::Kinematic
};

constexpr auto g_staticInfo = nc::physics::RigidBodyInfo{
    .type = nc::physics::BodyType::Static
};

TEST_F(RigidBodyTest, Constructor_staticEntityWithStaticBody_setsToCorrectBodyType)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_staticInfo};
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
}

TEST_F(RigidBodyTest, Constructor_staticEntityWithDynamicBody_overwritesBodyType)
{
    auto uut = nc::physics::RigidBody{g_staticEntity, g_shape, g_dynamicInfo};
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
}

TEST_F(RigidBodyTest, MoveOperations_transferRegistrationData)
{
    auto first = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    EXPECT_TRUE(first.IsInitialized());
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
}

TEST_F(RigidBodyTest, SimulationPropertyFunctions_updateInternalState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    auto apiBody = bodies.at(0);

    EXPECT_TRUE(uut.IsAwake());
    EXPECT_TRUE(apiBody->IsActive());
    uut.SetAwakeState(false);
    EXPECT_FALSE(uut.IsAwake());
    EXPECT_FALSE(apiBody->IsActive());
    uut.SetAwakeState(true);
    EXPECT_TRUE(uut.IsAwake());
    EXPECT_TRUE(apiBody->IsActive());

    EXPECT_FLOAT_EQ(g_dynamicInfo.friction, uut.GetFriction());
    uut.SetFriction(1.0f);
    EXPECT_FLOAT_EQ(1.0f, uut.GetFriction());
    EXPECT_FLOAT_EQ(1.0f, apiBody->GetFriction());

    EXPECT_FLOAT_EQ(g_dynamicInfo.restitution, uut.GetRestitution());
    uut.SetRestitution(1.0f);
    EXPECT_FLOAT_EQ(1.0f, uut.GetRestitution());
    EXPECT_FLOAT_EQ(1.0f, apiBody->GetRestitution());

    EXPECT_FLOAT_EQ(g_dynamicInfo.linearDamping, uut.GetLinearDamping());
    uut.SetLinearDamping(0.8f);
    EXPECT_FLOAT_EQ(0.8f, uut.GetLinearDamping());
    EXPECT_FLOAT_EQ(0.8f, apiBody->GetMotionProperties()->GetLinearDamping());

    EXPECT_FLOAT_EQ(g_dynamicInfo.angularDamping, uut.GetAngularDamping());
    uut.SetAngularDamping(0.7f);
    EXPECT_FLOAT_EQ(0.7f, uut.GetAngularDamping());
    EXPECT_FLOAT_EQ(0.7f, apiBody->GetMotionProperties()->GetAngularDamping());

    EXPECT_FLOAT_EQ(g_dynamicInfo.gravityMultiplier, uut.GetGravityMultiplier());
    uut.SetGravityMultiplier(2.0f);
    EXPECT_FLOAT_EQ(2.0f, uut.GetGravityMultiplier());
    EXPECT_FLOAT_EQ(2.0f, apiBody->GetMotionProperties()->GetGravityFactor());
}

TEST_F(RigidBodyTest, VelocityFunctions_dynamicBody_updateVelocities)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());

    uut.SetLinearVelocity(nc::Vector3::Splat(5.0f));
    uut.SetAngularVelocity(nc::Vector3::Splat(10.0f));
    EXPECT_EQ(nc::Vector3::Splat(5.0f), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Splat(10.0f), uut.GetAngularVelocity());

    uut.AddLinearVelocity(nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Splat(10.0f), uut.GetLinearVelocity());

    uut.AddVelocities(nc::Vector3::Splat(5.0f), nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Splat(15.0f), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Splat(15.0f), uut.GetAngularVelocity());

    uut.SetVelocities(nc::Vector3::Zero(), nc::Vector3::One());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::One(), uut.GetAngularVelocity());
}

TEST_F(RigidBodyTest, VelocityFunctions_kinematicBody_updateVelocities)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_kinematicInfo);
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());

    uut.SetLinearVelocity(nc::Vector3::Splat(5.0f));
    uut.SetAngularVelocity(nc::Vector3::Splat(10.0f));
    EXPECT_EQ(nc::Vector3::Splat(5.0f), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Splat(10.0f), uut.GetAngularVelocity());

    uut.AddLinearVelocity(nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Splat(10.0f), uut.GetLinearVelocity());

    uut.AddVelocities(nc::Vector3::Splat(5.0f), nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Splat(15.0f), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Splat(15.0f), uut.GetAngularVelocity());

    uut.SetVelocities(nc::Vector3::Zero(), nc::Vector3::One());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::One(), uut.GetAngularVelocity());
}

TEST_F(RigidBodyTest, VelocityFunctions_staticBody_triviallySucceed)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_staticInfo);
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());

    uut.SetLinearVelocity(nc::Vector3::Splat(5.0f));
    uut.SetAngularVelocity(nc::Vector3::Splat(10.0f));
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());

    uut.AddLinearVelocity(nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());

    uut.AddVelocities(nc::Vector3::Splat(5.0f), nc::Vector3::Splat(5.0f));
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());

    uut.SetVelocities(nc::Vector3::Zero(), nc::Vector3::One());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetLinearVelocity());
    EXPECT_EQ(nc::Vector3::Zero(), uut.GetAngularVelocity());
}

TEST_F(RigidBodyTest, ForceFunctions_dynamicBody_updateForces)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    auto apiBody = bodies.at(0);
    EXPECT_EQ(JPH::Vec3::sZero(), apiBody->GetAccumulatedForce());

    uut.AddForce(nc::Vector3::Splat(2.0f));
    uut.AddTorque(nc::Vector3::Splat(3.0f));
    EXPECT_EQ(JPH::Vec3::sReplicate(2.0f), apiBody->GetAccumulatedForce());
    EXPECT_EQ(JPH::Vec3::sReplicate(3.0f), apiBody->GetAccumulatedTorque());

    uut.AddForceAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
    EXPECT_EQ(JPH::Vec3::sReplicate(4.0f), apiBody->GetAccumulatedForce());
    EXPECT_NE(JPH::Vec3::sReplicate(3.0f), apiBody->GetAccumulatedTorque()); // just test that some torque is applied so we aren't dependent on their impl

    EXPECT_EQ(JPH::Vec3::sZero(), apiBody->GetLinearVelocity());
    EXPECT_EQ(JPH::Vec3::sZero(), apiBody->GetAngularVelocity());

    uut.AddImpulse(nc::Vector3::One());
    uut.AddAngularImpulse(nc::Vector3::One());
    EXPECT_LT(0.0f, apiBody->GetLinearVelocity().Length());
    EXPECT_LT(0.0f, apiBody->GetAngularVelocity().Length());

    uut.SetVelocities(nc::Vector3::Zero(), nc::Vector3::Zero());
    uut.AddImpulseAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
    EXPECT_LT(0.0f, apiBody->GetLinearVelocity().Length());
    EXPECT_LT(0.0f, apiBody->GetAngularVelocity().Length());
}

TEST_F(RigidBodyTest, ForceFunctions_kinematicBody_triviallySucceed)
{
    // Expect these have no effect, but don't crash. We can't call GetAccumulatedXXX funcs because they assert body is dynamic
    auto uut = CreateRigidBody(g_entity, g_shape, g_kinematicInfo);
    uut.AddForce(nc::Vector3::Splat(2.0f));
    uut.AddTorque(nc::Vector3::Splat(3.0f));
    uut.AddForceAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
    uut.AddImpulse(nc::Vector3::One());
    uut.AddAngularImpulse(nc::Vector3::One());
    uut.AddImpulseAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
}

TEST_F(RigidBodyTest, ForceFunctions_staticBody_triviallySucceed)
{
    // Expect these have no effect, but don't crash. We can't call GetAccumulatedXXX funcs because they assert body is dynamic
    auto uut = CreateRigidBody(g_entity, g_shape, g_staticInfo);
    uut.AddForce(nc::Vector3::Splat(2.0f));
    uut.AddTorque(nc::Vector3::Splat(3.0f));
    uut.AddForceAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
    uut.AddImpulse(nc::Vector3::One());
    uut.AddAngularImpulse(nc::Vector3::One());
    uut.AddImpulseAt(nc::Vector3::Splat(2.0f), nc::Vector3{0.5f, 0.3f, 0.5f});
}
