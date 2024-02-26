#include "gtest/gtest.h"
#include "physics/PhysicsPipelineTypes.h"
#include "ecs/Transform.h"

#include <array>

namespace
{
constexpr auto ColliderProperties               = nc::physics::ClientObjectProperties{false};
constexpr auto TriggerColliderProperties        = nc::physics::ClientObjectProperties{true};
constexpr auto PhysicsProperties                = nc::physics::ClientObjectProperties{false, false};
constexpr auto TriggerPhysicsProperties         = nc::physics::ClientObjectProperties{true,  false};
constexpr auto KinematicProperties              = nc::physics::ClientObjectProperties{false, true};
constexpr auto KinematicTriggerProperties       = nc::physics::ClientObjectProperties{true,  true};
constexpr auto AllProperties = std::array<nc::physics::ClientObjectProperties, 6>
{
    ColliderProperties,
    TriggerColliderProperties,
    PhysicsProperties,
    KinematicProperties,
    TriggerPhysicsProperties,
    KinematicTriggerProperties,
};

struct TestResults
{
    std::array<nc::physics::CollisionEventType, AllProperties.size()> results;

    TestResults(nc::physics::ClientObjectProperties first)
    {
        std::ranges::transform(AllProperties, results.begin(), [first](auto&& second)
        {
            return first.EventType(second);
        });
    }

    auto VsCollider()               const { return results.at(0); }
    auto VsTriggerCollider()        const { return results.at(1); }
    auto VsPhysics()                const { return results.at(2); }
    auto VsKinematic()              const { return results.at(3); }
    auto VsTriggerPhysics()         const { return results.at(4); }
    auto VsKinematicTrigger()       const { return results.at(5); }
};
} // anonymous namespace

TEST(ClientObjectProperties_unit_tests, FlagGetters)
{
    EXPECT_FALSE(ColliderProperties.IsTrigger());
    EXPECT_FALSE(ColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(ColliderProperties.IsKinematic());

    EXPECT_TRUE(TriggerColliderProperties.IsTrigger());
    EXPECT_FALSE(TriggerColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(TriggerColliderProperties.IsKinematic());

    EXPECT_FALSE(PhysicsProperties.IsTrigger());
    EXPECT_TRUE(PhysicsProperties.HasPhysicsBody());
    EXPECT_FALSE(PhysicsProperties.IsKinematic());

    EXPECT_TRUE(TriggerPhysicsProperties.IsTrigger());
    EXPECT_TRUE(TriggerPhysicsProperties.HasPhysicsBody());
    EXPECT_FALSE(TriggerPhysicsProperties.IsKinematic());

    EXPECT_FALSE(KinematicProperties.IsTrigger());
    EXPECT_TRUE(KinematicProperties.HasPhysicsBody());
    EXPECT_TRUE(KinematicProperties.IsKinematic());

    EXPECT_TRUE(KinematicTriggerProperties.IsTrigger());
    EXPECT_TRUE(KinematicTriggerProperties.HasPhysicsBody());
    EXPECT_TRUE(KinematicTriggerProperties.IsKinematic());
}

TEST(ClientObjectProperties_unit_tests, EventType_ColliderVsOthers)
{
    const auto results = TestResults{ColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_PhysicsVsOthers)
{
    const auto results = TestResults{PhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::TwoBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_KinematicVsOthers)
{
    const auto results = TestResults{KinematicProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_TriggerColliderVsOthers)
{
    const auto results = TestResults{TriggerColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_PhysicsTriggerVsOthers)
{
    const auto results = TestResults{TriggerPhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_KinematicTriggerVsOthers)
{
    const auto results = TestResults{KinematicTriggerProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
}
