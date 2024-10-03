#include "JoltApiFixture.inl"
#include "physics/jolt/CollisionQueryUtility.h"

class CollisionQueryUtilityTest : public JoltApiFixture
{
    protected:
        auto MakeQueryFilter(const nc::CollisionQueryFilter& filter)
        {
            return nc::physics::QueryFilter{filter, joltApi.physicsSystem.GetBodyLockInterfaceNoLock()};
        }

        auto MakeShapeCollector()
        {
            return nc::physics::ShapeCollector{joltApi.physicsSystem.GetBodyLockInterfaceNoLock()};
        }
};

TEST_F(CollisionQueryUtilityTest, QueryFilter_includeStatic_includeCorrectTypes)
{
    const auto filter = nc::CollisionQueryFilter{
        .includeStatic = true,
        .includeDynamic = false,
        .includeTrigger = false
    };

    const auto uut = MakeQueryFilter(filter);
    EXPECT_TRUE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Static));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Dynamic));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Trigger));

    EXPECT_TRUE(uut.ShouldCollide(nc::physics::ObjectLayer::Static));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Dynamic));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Trigger));
}

TEST_F(CollisionQueryUtilityTest, QueryFilter_includeDynamic_includeCorrectTypes)
{
    const auto filter = nc::CollisionQueryFilter{
        .includeStatic = false,
        .includeDynamic = true,
        .includeTrigger = false
    };

    const auto uut = MakeQueryFilter(filter);
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Static));
    EXPECT_TRUE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Dynamic));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Trigger));

    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Static));
    EXPECT_TRUE(uut.ShouldCollide(nc::physics::ObjectLayer::Dynamic));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Trigger));
}

TEST_F(CollisionQueryUtilityTest, QueryFilter_includeTrigger_includeCorrectTypes)
{
    const auto filter = nc::CollisionQueryFilter{
        .includeStatic = false,
        .includeDynamic = false,
        .includeTrigger = true
    };

    const auto uut = MakeQueryFilter(filter);
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Static));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Dynamic));
    EXPECT_TRUE(uut.ShouldCollide(nc::physics::BroadPhaseLayer::Trigger));

    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Static));
    EXPECT_FALSE(uut.ShouldCollide(nc::physics::ObjectLayer::Dynamic));
    EXPECT_TRUE(uut.ShouldCollide(nc::physics::ObjectLayer::Trigger));
}

TEST_F(CollisionQueryUtilityTest, QueryFilter_entityFilter_includesAllowedBodies)
{
    const auto filter = nc::CollisionQueryFilter{
        .entityFilter = [](nc::Entity hit) {
            return hit.Layer() == 42;
        },
        .includeStatic = true,
        .includeDynamic = true,
        .includeTrigger = true
    };

    const auto uut = MakeQueryFilter(filter);
    const auto includedEntity = nc::Entity{0, 42, 0};
    const auto excludedEntity = nc::Entity{1, 0, 0};
    auto includedBody = CreateBody();
    auto excludedBody = CreateBody();
    includedBody->SetUserData(nc::Entity::Hash{}(includedEntity));
    excludedBody->SetUserData(nc::Entity::Hash{}(excludedEntity));

    EXPECT_TRUE(uut.ShouldCollide(includedBody->GetID()));
    EXPECT_FALSE(uut.ShouldCollide(excludedBody->GetID()));

    EXPECT_TRUE(uut.ShouldCollideLocked(*includedBody));
    EXPECT_FALSE(uut.ShouldCollideLocked(*excludedBody));

    DestroyBody(includedBody);
    DestroyBody(excludedBody);
}

TEST_F(CollisionQueryUtilityTest, QueryFilter_nullEntityFilter_throws)
{
    const auto filter = nc::CollisionQueryFilter{
        .entityFilter = nullptr
    };

    EXPECT_THROW(MakeQueryFilter(filter), nc::NcError);
}

TEST_F(CollisionQueryUtilityTest, ShapeCollector_convertsHits)
{
    const auto entity = nc::Entity{1, 2, 0};
    auto body = CreateBody();
    body->SetUserData(nc::Entity::Hash{}(entity));

    const auto expected = JPH::CollideShapeResult{
        JPH::Vec3{1.0f, 0.0f, 0.0f},
        JPH::Vec3{-1.0f, 0.0f, 0.0f},
        JPH::Vec3{-1.0f, 0.0f, 0.0f},
        0.5f,
        JPH::SubShapeID{},
        JPH::SubShapeID{},
        body->GetID()
    };

    auto uut = MakeShapeCollector();
    uut.AddHit(expected);
    const auto actual = uut.ExtractHits();
    ASSERT_EQ(1ull, actual.size());
    const auto& hit = actual.at(0);

    EXPECT_EQ(entity, hit.hit);
    EXPECT_EQ(nc::physics::ToVector3(expected.mContactPointOn2), hit.point);
    EXPECT_FLOAT_EQ(expected.mPenetrationDepth, hit.depth);

    const auto expectedNormal = (expected.mContactPointOn2 - expected.mContactPointOn1).Normalized();
    EXPECT_EQ(nc::physics::ToVector3(expectedNormal), hit.collisionNormal);

    DestroyBody(body);
}
