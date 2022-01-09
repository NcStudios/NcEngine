#include "gtest/gtest.h"
#include "physics/PhysicsPipelineTypes.h"
#include "ecs/component/Transform.h"

using namespace nc;

namespace
{
    const auto ColliderProperties                = physics::ClientObjectProperties{false, true,  false};
    const auto ColliderTriggerProperties         = physics::ClientObjectProperties{true,  true,  false};
    const auto PhysicsProperties                 = physics::ClientObjectProperties{false, false, false};
    const auto PhysicsTriggerProperties          = physics::ClientObjectProperties{true,  false, false};
    const auto PhysicsKinematicProperties        = physics::ClientObjectProperties{false, false, true};
    const auto PhysicsKinematicTriggerProperties = physics::ClientObjectProperties{true,  false, true};
}

namespace nc
{
    auto Transform::TransformationMatrix() const -> DirectX::FXMMATRIX { return m_worldMatrix; }
}

TEST(ClientObjectProperties_unit_tests, GetEventType_ColliderVsOthers)
{
    auto colliderVsCollider = ColliderProperties.EventType(ColliderProperties);
    auto colliderVsPhysics = ColliderProperties.EventType(PhysicsProperties);
    auto colliderVsKinematicPhysics = ColliderProperties.EventType(PhysicsKinematicProperties);
    auto colliderVsColliderTrigger = ColliderProperties.EventType(ColliderTriggerProperties);
    auto colliderVsPhysicsTrigger = ColliderProperties.EventType(PhysicsTriggerProperties);
    auto colliderVsKinematicPhysicsTrigger = ColliderProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(colliderVsCollider, physics::CollisionEventType::None);
    EXPECT_EQ(colliderVsPhysics, physics::CollisionEventType::SecondBodyPhysics);
    EXPECT_EQ(colliderVsKinematicPhysics, physics::CollisionEventType::None);
    EXPECT_EQ(colliderVsColliderTrigger, physics::CollisionEventType::None);
    EXPECT_EQ(colliderVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(colliderVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, GetEventType_PhysicsVsOthers)
{
    auto physicsVsCollider = PhysicsProperties.EventType(ColliderProperties);
    auto physicsVsPhysics = PhysicsProperties.EventType(PhysicsProperties);
    auto physicsVsKinematicPhysics = PhysicsProperties.EventType(PhysicsKinematicProperties);
    auto physicsVsColliderTrigger = PhysicsProperties.EventType(ColliderTriggerProperties);
    auto physicsVsPhysicsTrigger = PhysicsProperties.EventType(PhysicsTriggerProperties);
    auto physicsVsKinematicPhysicsTrigger = PhysicsProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(physicsVsCollider, physics::CollisionEventType::FirstBodyPhysics);
    EXPECT_EQ(physicsVsPhysics, physics::CollisionEventType::TwoBodyPhysics);
    EXPECT_EQ(physicsVsKinematicPhysics, physics::CollisionEventType::FirstBodyPhysics);
    EXPECT_EQ(physicsVsColliderTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, GetEventType_KinematicPhysicsVsOthers)
{
    auto kinematicPhysicsVsCollider = PhysicsKinematicProperties.EventType(ColliderProperties);
    auto kinematicPhysicsVsPhysics = PhysicsKinematicProperties.EventType(PhysicsProperties);
    auto kinematicPhysicsVsKinematicPhysics = PhysicsKinematicProperties.EventType(PhysicsKinematicProperties);
    auto kinematicPhysicsVsColliderTrigger = PhysicsKinematicProperties.EventType(ColliderTriggerProperties);
    auto kinematicPhysicsVsPhysicsTrigger = PhysicsKinematicProperties.EventType(PhysicsTriggerProperties);
    auto kinematicPhysicsVsKinematicPhysicsTrigger = PhysicsKinematicProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(kinematicPhysicsVsCollider, physics::CollisionEventType::None);
    EXPECT_EQ(kinematicPhysicsVsPhysics, physics::CollisionEventType::SecondBodyPhysics);
    EXPECT_EQ(kinematicPhysicsVsKinematicPhysics, physics::CollisionEventType::None);
    EXPECT_EQ(kinematicPhysicsVsColliderTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, GetEventType_ColliderTriggerVsOthers)
{
    auto colliderTriggerVsCollider = ColliderTriggerProperties.EventType(ColliderProperties);
    auto colliderTriggerVsPhysics = ColliderTriggerProperties.EventType(PhysicsProperties);
    auto colliderTriggerVsKinematicPhysics = ColliderTriggerProperties.EventType(PhysicsKinematicProperties);
    auto colliderTriggerVsColliderTrigger = ColliderTriggerProperties.EventType(ColliderTriggerProperties);
    auto colliderTriggerVsPhysicsTrigger = ColliderTriggerProperties.EventType(PhysicsTriggerProperties);
    auto colliderTriggerVsKinematicPhysicsTrigger = ColliderTriggerProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(colliderTriggerVsCollider, physics::CollisionEventType::None);
    EXPECT_EQ(colliderTriggerVsPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(colliderTriggerVsKinematicPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(colliderTriggerVsColliderTrigger, physics::CollisionEventType::None);
    EXPECT_EQ(colliderTriggerVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(colliderTriggerVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, GetEventType_PhysicsTriggerVsOthers)
{
    auto physicsTriggerVsCollider = PhysicsTriggerProperties.EventType(ColliderProperties);
    auto physicsTriggerVsPhysics = PhysicsTriggerProperties.EventType(PhysicsProperties);
    auto physicsTriggerVsKinematicPhysics = PhysicsTriggerProperties.EventType(PhysicsKinematicProperties);
    auto physicsTriggerVsColliderTrigger = PhysicsTriggerProperties.EventType(ColliderTriggerProperties);
    auto physicsTriggerVsPhysicsTrigger = PhysicsTriggerProperties.EventType(PhysicsTriggerProperties);
    auto physicsTriggerVsKinematicPhysicsTrigger = PhysicsTriggerProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(physicsTriggerVsCollider, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsTriggerVsPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsTriggerVsKinematicPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsTriggerVsColliderTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsTriggerVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(physicsTriggerVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, GetEventType_KinematicPhysicsTriggerVsOthers)
{
    auto kinematicPhysicsTriggerVsCollider = PhysicsKinematicTriggerProperties.EventType(ColliderProperties);
    auto kinematicPhysicsTriggerVsPhysics = PhysicsKinematicTriggerProperties.EventType(PhysicsProperties);
    auto kinematicPhysicsTriggerVsKinematicPhysics = PhysicsKinematicTriggerProperties.EventType(PhysicsKinematicProperties);
    auto kinematicPhysicsTriggerVsColliderTrigger = PhysicsKinematicTriggerProperties.EventType(ColliderTriggerProperties);
    auto kinematicPhysicsTriggerVsPhysicsTrigger = PhysicsKinematicTriggerProperties.EventType(PhysicsTriggerProperties);
    auto kinematicPhysicsTriggerVsKinematicPhysicsTrigger = PhysicsKinematicTriggerProperties.EventType(PhysicsKinematicTriggerProperties);

    EXPECT_EQ(kinematicPhysicsTriggerVsCollider, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsTriggerVsPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsTriggerVsKinematicPhysics, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsTriggerVsColliderTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsTriggerVsPhysicsTrigger, physics::CollisionEventType::Trigger);
    EXPECT_EQ(kinematicPhysicsTriggerVsKinematicPhysicsTrigger, physics::CollisionEventType::Trigger);
}

TEST(ClientObjectProperties_unit_tests, IsTrigger_TriggerObject_ReturnsTrue)
{
    EXPECT_TRUE(ColliderTriggerProperties.IsTrigger());
}

TEST(ClientObjectProperties_unit_tests, IsTrigger_NonTriggerObject_ReturnsFalse)
{
    EXPECT_FALSE(PhysicsProperties.IsTrigger());
}

TEST(ClientObjectProperties_unit_tests, HasPhysicsBody_PhysicsObject_ReturnsTrue)
{
    EXPECT_TRUE(PhysicsProperties.HasPhysicsBody());
}

TEST(ClientObjectProperties_unit_tests, HasPhysicsBody_TriggerObject_ReturnsFalse)
{
    EXPECT_FALSE(ColliderProperties.HasPhysicsBody());
}

TEST(ClientObjectProperties_unit_tests, IsKinematic_KinematicObject_ReturnsTrue)
{
    EXPECT_TRUE(PhysicsKinematicProperties.IsKinematic());
}

TEST(ClientObjectProperties_unit_tests, IsKinematic_NonKinematicObject_ReturnsFalse)
{
    EXPECT_FALSE(ColliderTriggerProperties.IsKinematic());
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}