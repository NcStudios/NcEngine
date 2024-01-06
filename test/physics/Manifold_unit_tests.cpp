#include "gtest/gtest.h"
#include "physics/PhysicsPipelineTypes.h"
#include "ecs/Transform.h"

using namespace nc;

namespace
{
    constexpr auto entity1 = Entity{1u, 0u, Entity::Flags::None};
    constexpr auto entity2 = Entity{2u, 0u, Entity::Flags::None};

    auto CreateContact(float depth = 0.1f)
    {
        return physics::Contact
        {
            .worldPointA = Vector3::Zero(),
            .worldPointB = Vector3::Zero(),
            .localPointA = Vector3::Zero(),
            .localPointB = Vector3::Zero(),
            .normal = Vector3::One(),
            .depth = depth
        };
    }

    auto CreateContact(Vector3 point, float depth = 0.1f)
    {
        return physics::Contact
        {
            .worldPointA = point,
            .worldPointB = point,
            .localPointA = point,
            .localPointB = point,
            .normal = Vector3::One(),
            .depth = depth
        };
    }
}

TEST(Manifold_unit_tests, AddContact_UpdatesEventStateToPersisting)
{
    auto manifold = physics::Manifold{entity1, entity2, physics::CollisionEventType::TwoBodyPhysics, CreateContact()};
    EXPECT_EQ(manifold.Event().state, physics::NarrowEvent::State::New);
    manifold.AddContact(CreateContact(0.1f));
    EXPECT_EQ(manifold.Event().state, physics::NarrowEvent::State::Persisting);
}

TEST(Manifold_unit_tests, AddContact_LessThanCapacity_AppendsToContactList)
{
    auto manifold = physics::Manifold{entity1, entity2, physics::CollisionEventType::TwoBodyPhysics, CreateContact()};
    EXPECT_EQ(manifold.Contacts().size(), 1u);
    manifold.AddContact(CreateContact(0.1f));
    EXPECT_EQ(manifold.Contacts().size(), 2u);
}

TEST(Manifold_unit_tests, AddContact_GreaterThanCapacity_GoodFitReplacesWorstFit)
{
    auto c1 = CreateContact(Vector3{ 1.0f, 0.0f,  1.0f}, 0.1f);
    auto c2 = CreateContact(Vector3{ 0.0f, 0.0f,  0.0f}, 0.3f); // should replace this one
    auto c3 = CreateContact(Vector3{-1.0f, 0.0f,  1.0f}, 0.2f);
    auto c4 = CreateContact(Vector3{ 1.0f, 0.0f, -1.0f}, 0.4f);

    auto manifold = physics::Manifold{entity1, entity2, physics::CollisionEventType::TwoBodyPhysics, c1};
    manifold.AddContact(c2);
    manifold.AddContact(c3);
    manifold.AddContact(c4);

    auto expectedPosition = Vector3{-1.0f, 0.0f, -1.0f};
    auto expectedDepth = 0.15f;
    manifold.AddContact(CreateContact(expectedPosition, expectedDepth));

    auto contacts = manifold.Contacts();
    ASSERT_EQ(contacts.size(), 4u);
    const auto& actual = contacts[1];
    EXPECT_EQ(actual.depth, expectedDepth);
    EXPECT_EQ(actual.worldPointA, expectedPosition);
}

TEST(Manifold_unit_tests, AddContact_GreaterThanCapacity_BadFitReplacesFirst)
{
    auto c1 = CreateContact(Vector3{ 1.0f, 0.0f,  1.0f}, 0.1f);
    auto c2 = CreateContact(Vector3{-1.0f, 0.0f, -1.0f}, 0.15f); // should replace this one
    auto c3 = CreateContact(Vector3{-1.0f, 0.0f,  1.0f}, 0.2f);
    auto c4 = CreateContact(Vector3{ 1.0f, 0.0f, -1.0f}, 0.4f);

    auto manifold = physics::Manifold{entity1, entity2, physics::CollisionEventType::TwoBodyPhysics, c1};
    manifold.AddContact(c2);
    manifold.AddContact(c3);
    manifold.AddContact(c4);

    auto expectedPosition = Vector3{0.1f, 0.0f, 0.1f};
    auto expectedDepth = 0.15f;
    manifold.AddContact(CreateContact(expectedPosition, expectedDepth));

    auto contacts = manifold.Contacts();
    ASSERT_EQ(contacts.size(), 4u);
    EXPECT_EQ(contacts[0].depth, expectedDepth);
    EXPECT_EQ(contacts[0].worldPointA, expectedPosition);
}

TEST(Manifold_unit_tests, GetDeepestContact_FromEachPosition_ReturnsContact)
{
    auto c1 = CreateContact(0.5f);
    auto c2 = CreateContact(0.1f);
    auto manifold = physics::Manifold{entity1, entity2, physics::CollisionEventType::TwoBodyPhysics, c1};
    EXPECT_EQ(manifold.DeepestContact().depth, c1.depth);
    manifold.AddContact(c2);
    EXPECT_EQ(manifold.DeepestContact().depth, c1.depth);
    manifold.AddContact(c2);
    EXPECT_EQ(manifold.DeepestContact().depth, c1.depth);
    manifold.AddContact(c2);
    EXPECT_EQ(manifold.DeepestContact().depth, c1.depth);
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
