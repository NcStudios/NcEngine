#include "jolt/JoltApiFixture.inl"
#include "ncengine/physics/RigidBody.h"

#include "physics2/jolt/BodyManager.h"
#include "physics2/jolt/ComponentContext.h"
#include "physics2/jolt/ConstraintManager.h"
#include "physics2/jolt/ShapeFactory.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"

#include <vector>

namespace nc::physics
{
struct BodyManager::Connections {};

BodyManager::BodyManager(ecs::ComponentPool<Transform>& transformPool,
                         ecs::ComponentPool<RigidBody>&,
                         uint32_t maxEntities,
                         JPH::PhysicsSystem& physicsSystem,
                         ShapeFactory& shapeFactory,
                         ConstraintManager& constraintManager)
    : m_transformPool{&transformPool},
      m_bodies{maxEntities, maxEntities},
      m_bodyFactory{physicsSystem.GetBodyInterfaceNoLock(), shapeFactory},
      m_ctx{std::make_unique<ComponentContext>(
        physicsSystem.GetBodyInterfaceNoLock(),
        shapeFactory,
        constraintManager
      )}
{
    nc::physics::RigidBody::SetContext(m_ctx.get());
}

void BodyManager::AddBody(RigidBody& added)
{
    const auto matrix = DirectX::XMMatrixIdentity();
    auto [handle, _1, _2] = m_bodyFactory.MakeBody(added, matrix);
    m_ctx->interface.AddBody(handle->GetID(), JPH::EActivation::Activate);
    added.SetHandle(handle);
    m_bodies.emplace(added.GetEntity().Index(), handle->GetID());
}

void BodyManager::RemoveBody(Entity toRemove)
{
    const auto bodyId = m_bodies.at(toRemove.Index());
    m_bodies.erase(toRemove.Index());
    m_ctx->interface.RemoveBody(bodyId);
    m_ctx->interface.DestroyBody(bodyId);
}

BodyManager::~BodyManager() noexcept = default;
} // namespace nc::physics

class RigidBodyTest : public JoltApiFixture
{
    protected:
        RigidBodyTest()
            : transformPool{10, nc::ComponentHandler<nc::Transform>{}},
              rigidBodyPool{10ull, nc::ComponentHandler<nc::physics::RigidBody>{}},
              constraintManager{joltApi.physicsSystem, 10},
              bodyManager{
                  transformPool,
                  rigidBodyPool,
                  10,
                  joltApi.physicsSystem,
                  shapeFactory,
                  constraintManager
              }
        {
        }

    public:
        nc::ecs::ComponentPool<nc::Transform> transformPool;
        nc::ecs::ComponentPool<nc::physics::RigidBody> rigidBodyPool;
        nc::physics::ShapeFactory shapeFactory;
        nc::physics::ConstraintManager constraintManager;
        nc::physics::BodyManager bodyManager;
        std::vector<JPH::Body*> bodies;

        auto CreateRigidBody(nc::Entity entity,
                             const nc::physics::Shape& shape,
                             const nc::physics::RigidBodyInfo& info)
        {
            auto body = nc::physics::RigidBody{entity, shape, info};
            bodyManager.AddBody(body);
            bodies.push_back(reinterpret_cast<JPH::Body*>(body.GetHandle()));
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

TEST_F(RigidBodyTest, Constructor_triggerWithContinuousDetection_disablesContinuosDetection)
{
    auto uut = nc::physics::RigidBody{
        g_entity,
        g_shape,
        nc::physics::RigidBodyInfo{
            .flags = nc::physics::RigidBodyFlags::Trigger |
                     nc::physics::RigidBodyFlags::ContinuousDetection
        }
    };

    EXPECT_FALSE(uut.UseContinuousDetection());
    EXPECT_TRUE(uut.IsTrigger());
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
    first = std::move(first);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    EXPECT_TRUE(first.GetEntity().Valid());
    EXPECT_TRUE(first.IsInitialized());
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

TEST_F(RigidBodyTest, RigidBodyFlagFunctions_validFlags_setExpectedState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    auto body = static_cast<JPH::Body*>(uut.GetHandle());

    uut.SetTrigger(true);
    EXPECT_TRUE(uut.IsTrigger());
    EXPECT_TRUE(body->IsSensor());

    uut.SetTrigger(false);
    EXPECT_FALSE(uut.IsTrigger());
    EXPECT_FALSE(body->IsSensor());

    uut.UseContinuousDetection(true);
    EXPECT_TRUE(uut.UseContinuousDetection());
    EXPECT_EQ(JPH::EMotionQuality::LinearCast, body->GetMotionProperties()->GetMotionQuality());

    uut.UseContinuousDetection(false);
    EXPECT_FALSE(uut.UseContinuousDetection());
    EXPECT_EQ(JPH::EMotionQuality::Discrete, body->GetMotionProperties()->GetMotionQuality());

    uut.IgnoreTransformScaling(true);
    EXPECT_TRUE(uut.IgnoreTransformScaling());
    EXPECT_FALSE(uut.ScalesWithTransform());

    uut.IgnoreTransformScaling(false);
    EXPECT_FALSE(uut.IgnoreTransformScaling());
    EXPECT_TRUE(uut.ScalesWithTransform());
}

TEST_F(RigidBodyTest, RigidBodyFlagFunctions_incompatibleFlags_doesNotModifyState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    auto body = static_cast<JPH::Body*>(uut.GetHandle());

    uut.SetTrigger(true);
    uut.UseContinuousDetection(true);
    EXPECT_TRUE(uut.IsTrigger());
    EXPECT_TRUE(body->IsSensor());
    EXPECT_FALSE(uut.UseContinuousDetection());
    EXPECT_EQ(JPH::EMotionQuality::Discrete, body->GetMotionProperties()->GetMotionQuality());

    uut.SetTrigger(false);
    uut.UseContinuousDetection(true);
    uut.SetTrigger(true);
    EXPECT_FALSE(uut.IsTrigger());
    EXPECT_FALSE(body->IsSensor());
    EXPECT_TRUE(uut.UseContinuousDetection());
    EXPECT_EQ(JPH::EMotionQuality::LinearCast, body->GetMotionProperties()->GetMotionQuality());
}

TEST_F(RigidBodyTest, SetBody_dynamicToStatic_updateInternalState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_dynamicInfo);
    const auto apiBody = static_cast<JPH::Body*>(uut.GetHandle());

    uut.SetBodyType(nc::physics::BodyType::Static);
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
    EXPECT_EQ(JPH::EMotionType::Static, apiBody->GetMotionType());
    EXPECT_EQ(nc::physics::ObjectLayer::Static, apiBody->GetObjectLayer());
    EXPECT_EQ(nc::physics::BroadPhaseLayer::Static, apiBody->GetBroadPhaseLayer());
}

TEST_F(RigidBodyTest, SetBody_kinematicToStatic_updateInternalState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_kinematicInfo);
    const auto apiBody = static_cast<JPH::Body*>(uut.GetHandle());

    uut.SetBodyType(nc::physics::BodyType::Static);
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
    EXPECT_EQ(JPH::EMotionType::Static, apiBody->GetMotionType());
    EXPECT_EQ(nc::physics::ObjectLayer::Static, apiBody->GetObjectLayer());
    EXPECT_EQ(nc::physics::BroadPhaseLayer::Static, apiBody->GetBroadPhaseLayer());
}

TEST_F(RigidBodyTest, SetBody_staticToDynamic_updateInternalState)
{
    auto uut = CreateRigidBody(g_entity, g_shape, g_staticInfo);
    const auto apiBody = static_cast<JPH::Body*>(uut.GetHandle());

    uut.SetBodyType(nc::physics::BodyType::Dynamic);
    EXPECT_EQ(nc::physics::BodyType::Dynamic, uut.GetBodyType());
    EXPECT_EQ(JPH::EMotionType::Dynamic, apiBody->GetMotionType());
    EXPECT_EQ(nc::physics::ObjectLayer::Dynamic, apiBody->GetObjectLayer());
    EXPECT_EQ(nc::physics::BroadPhaseLayer::Dynamic, apiBody->GetBroadPhaseLayer());
}

TEST_F(RigidBodyTest, SetBody_staticEntity_doesNotModifyState)
{
    auto uut = CreateRigidBody(g_staticEntity, g_shape, g_staticInfo);
    const auto apiBody = static_cast<JPH::Body*>(uut.GetHandle());

    EXPECT_NO_THROW(uut.SetBodyType(nc::physics::BodyType::Dynamic));
    EXPECT_EQ(nc::physics::BodyType::Static, uut.GetBodyType());
    EXPECT_EQ(JPH::EMotionType::Static, apiBody->GetMotionType());
    EXPECT_EQ(nc::physics::ObjectLayer::Static, apiBody->GetObjectLayer());
    EXPECT_EQ(nc::physics::BroadPhaseLayer::Static, apiBody->GetBroadPhaseLayer());
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