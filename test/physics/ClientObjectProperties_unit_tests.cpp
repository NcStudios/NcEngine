#include "gtest/gtest.h"
#include "physics/PhysicsPipelineTypes.h"
#include "ecs/Transform.h"

#include <array>

namespace
{
constexpr auto ColliderProperties               = nc::physics::ClientObjectProperties{false, false};
constexpr auto StaticColliderProperties         = nc::physics::ClientObjectProperties{true, false};
constexpr auto TriggerColliderProperties        = nc::physics::ClientObjectProperties{false, true};
constexpr auto StaticTriggerColliderProperties  = nc::physics::ClientObjectProperties{true, true};
constexpr auto PhysicsProperties                = nc::physics::ClientObjectProperties{false, false, false};
constexpr auto StaticPhysicsProperties          = nc::physics::ClientObjectProperties{true, false, true}; // static must be kinematic
constexpr auto TriggerPhysicsProperties         = nc::physics::ClientObjectProperties{false, true,  false};
constexpr auto StaticTriggerPhysicsProperties   = nc::physics::ClientObjectProperties{true, true,  true};
constexpr auto KinematicProperties              = nc::physics::ClientObjectProperties{false, false, true};
constexpr auto StaticKinematicProperties        = nc::physics::ClientObjectProperties{true, false, true};
constexpr auto KinematicTriggerProperties       = nc::physics::ClientObjectProperties{false, true,  true};
constexpr auto StaticKinematicTriggerProperties = nc::physics::ClientObjectProperties{true, true,  true};
constexpr auto AllProperties = std::array<nc::physics::ClientObjectProperties, 12>
{
    ColliderProperties,
    StaticColliderProperties,
    TriggerColliderProperties,
    StaticTriggerColliderProperties,
    PhysicsProperties,
    StaticPhysicsProperties,
    KinematicProperties,
    StaticKinematicProperties,
    TriggerPhysicsProperties,
    StaticTriggerPhysicsProperties,
    KinematicTriggerProperties,
    StaticKinematicTriggerProperties
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
    auto VsStaticCollider()         const { return results.at(1); }
    auto VsTriggerCollider()        const { return results.at(2); }
    auto VsStaticTriggerCollider()  const { return results.at(3); }
    auto VsPhysics()                const { return results.at(4); }
    auto VsStaticPhysics()          const { return results.at(5); }
    auto VsKinematic()              const { return results.at(6); }
    auto VsStaticKinematic()        const { return results.at(7); }
    auto VsTriggerPhysics()         const { return results.at(8); }
    auto VsStaticTriggerPhysics()   const { return results.at(9); }
    auto VsKinematicTrigger()       const { return results.at(10); }
    auto VsStaticKinematicTrigger() const { return results.at(11); }
};
} // anonymous namespace

TEST(ClientObjectProperties_unit_tests, FlagGetters)
{
    EXPECT_FALSE(ColliderProperties.IsTrigger());
    EXPECT_FALSE(ColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(ColliderProperties.IsKinematic());
    EXPECT_FALSE(ColliderProperties.IsStatic());

    EXPECT_TRUE(TriggerColliderProperties.IsTrigger());
    EXPECT_FALSE(TriggerColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(TriggerColliderProperties.IsKinematic());
    EXPECT_FALSE(TriggerColliderProperties.IsStatic());

    EXPECT_FALSE(PhysicsProperties.IsTrigger());
    EXPECT_TRUE(PhysicsProperties.HasPhysicsBody());
    EXPECT_FALSE(PhysicsProperties.IsKinematic());
    EXPECT_FALSE(PhysicsProperties.IsStatic());

    EXPECT_TRUE(TriggerPhysicsProperties.IsTrigger());
    EXPECT_TRUE(TriggerPhysicsProperties.HasPhysicsBody());
    EXPECT_FALSE(TriggerPhysicsProperties.IsKinematic());
    EXPECT_FALSE(TriggerPhysicsProperties.IsStatic());

    EXPECT_FALSE(KinematicProperties.IsTrigger());
    EXPECT_TRUE(KinematicProperties.HasPhysicsBody());
    EXPECT_TRUE(KinematicProperties.IsKinematic());
    EXPECT_FALSE(KinematicProperties.IsStatic());

    EXPECT_TRUE(KinematicTriggerProperties.IsTrigger());
    EXPECT_TRUE(KinematicTriggerProperties.HasPhysicsBody());
    EXPECT_TRUE(KinematicTriggerProperties.IsKinematic());
    EXPECT_FALSE(KinematicTriggerProperties.IsStatic());

    EXPECT_FALSE(StaticColliderProperties.IsTrigger());
    EXPECT_FALSE(StaticColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(StaticColliderProperties.IsKinematic());
    EXPECT_TRUE(StaticColliderProperties.IsStatic());

    EXPECT_TRUE(StaticTriggerColliderProperties.IsTrigger());
    EXPECT_FALSE(StaticTriggerColliderProperties.HasPhysicsBody());
    EXPECT_FALSE(StaticTriggerColliderProperties.IsKinematic());
    EXPECT_TRUE(StaticTriggerColliderProperties.IsStatic());

    EXPECT_FALSE(StaticPhysicsProperties.IsTrigger());
    EXPECT_TRUE(StaticPhysicsProperties.HasPhysicsBody());
    EXPECT_TRUE(StaticPhysicsProperties.IsKinematic());
    EXPECT_TRUE(StaticPhysicsProperties.IsStatic());

    EXPECT_TRUE(StaticTriggerPhysicsProperties.IsTrigger());
    EXPECT_TRUE(StaticTriggerPhysicsProperties.HasPhysicsBody());
    EXPECT_TRUE(StaticTriggerPhysicsProperties.IsKinematic());
    EXPECT_TRUE(StaticTriggerPhysicsProperties.IsStatic());

    EXPECT_FALSE(StaticKinematicProperties.IsTrigger());
    EXPECT_TRUE(StaticKinematicProperties.HasPhysicsBody());
    EXPECT_TRUE(StaticKinematicProperties.IsKinematic());
    EXPECT_TRUE(StaticKinematicProperties.IsStatic());

    EXPECT_TRUE(StaticKinematicTriggerProperties.IsTrigger());
    EXPECT_TRUE(StaticKinematicTriggerProperties.HasPhysicsBody());
    EXPECT_TRUE(StaticKinematicTriggerProperties.IsKinematic());
    EXPECT_TRUE(StaticKinematicTriggerProperties.IsStatic());
}

TEST(ClientObjectProperties_unit_tests, EventType_ColliderVsOthers)
{
    const auto results = TestResults{ColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_StaticColliderVsOthers)
{
    const auto results = TestResults{StaticColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_PhysicsVsOthers)
{
    const auto results = TestResults{PhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::TwoBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::FirstBodyPhysics, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}
TEST(ClientObjectProperties_unit_tests, EventType_StaticPhysicsVsOthers)
{
    const auto results = TestResults{StaticPhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_KinematicVsOthers)
{
    const auto results = TestResults{KinematicProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_StaticKinematicVsOthers)
{
    const auto results = TestResults{StaticKinematicProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::SecondBodyPhysics, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_TriggerColliderVsOthers)
{
    const auto results = TestResults{TriggerColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_StaticTriggerColliderVsOthers)
{
    const auto results = TestResults{StaticTriggerColliderProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_PhysicsTriggerVsOthers)
{
    const auto results = TestResults{TriggerPhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_StaticPhysicsTriggerVsOthers)
{
    const auto results = TestResults{StaticTriggerPhysicsProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_KinematicTriggerVsOthers)
{
    const auto results = TestResults{KinematicTriggerProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsStaticKinematicTrigger());
}

TEST(ClientObjectProperties_unit_tests, EventType_StaticKinematicTriggerVsOthers)
{
    const auto results = TestResults{StaticKinematicTriggerProperties};
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerCollider());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematic());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticTriggerPhysics());
    EXPECT_EQ(nc::physics::CollisionEventType::Trigger, results.VsKinematicTrigger());
    EXPECT_EQ(nc::physics::CollisionEventType::None, results.VsStaticKinematicTrigger());
}
