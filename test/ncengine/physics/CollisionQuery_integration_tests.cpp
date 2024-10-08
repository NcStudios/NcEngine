#include "jolt/JoltApiFixture.inl"
#include "ncengine/physics/CollisionQuery.h"
#include "physics/jolt/CollisionQueryManager.h"
#include "physics/jolt/ShapeFactory.h"

class CollisionQueryTest : public JoltApiFixture
{
    private:
        nc::physics::ShapeFactory m_shapeFactory;
        nc::physics::CollisionQueryManager m_queryManager;

    protected:
        CollisionQueryTest()
            : m_shapeFactory{},
              m_queryManager{
                joltApi.physicsSystem.GetNarrowPhaseQuery(),
                joltApi.physicsSystem.GetBodyLockInterfaceNoLock(),
                m_shapeFactory
              }
        {
        }

        auto CreateBody(nc::Entity entity,
                        const nc::Shape& shape,
                        const JPH::Vec3& position = JPH::Vec3::sZero(),
                        const JPH::Quat& rotation = JPH::Quat::sIdentity()) -> JPH::Body*
        {
            auto bodySettings = JPH::BodyCreationSettings{
                m_shapeFactory.MakeShape(shape, JPH::Vec3::sReplicate(1.0f)),
                position,
                rotation,
                JPH::EMotionType::Dynamic,
                JPH::ObjectLayer{0}
            };

            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            auto body = interface.CreateBody(bodySettings);
            interface.AddBody(body->GetID(), JPH::EActivation::Activate);
            body->SetUserData(nc::Entity::Hash{}(entity));
            return body;
        }
};

const auto g_sphere = nc::Shape::MakeSphere();
constexpr auto g_entity1 = nc::Entity{1, 0, 0};
constexpr auto g_entity2 = nc::Entity{2, 0, 0};
constexpr auto g_entity3 = nc::Entity{3, 0, 0};

TEST_F(CollisionQueryTest, CastRay_occludedObject_findsFirst)
{
    const auto body1 = CreateBody(g_entity1, g_sphere, JPH::Vec3{-4.0f, 0.0f, 0.0f});
    const auto body2 = CreateBody(g_entity2, g_sphere, JPH::Vec3{-8.0f, 0.0f, 0.0f});
    const auto body3 = CreateBody(g_entity2, g_sphere, JPH::Vec3{-12.0f, 0.0f, 0.0f});
    const auto ray = nc::Ray{nc::Vector3::Zero(), nc::Vector3::Left() * 15.0f};

    auto uut = nc::CollisionQuery();
    const auto result = uut.CastRay(ray);
    EXPECT_EQ(g_entity1, result.hit);
    EXPECT_EQ(nc::Vector3(-3.5f, 0.0f, 0.0f), result.point);
    EXPECT_EQ(nc::Vector3::Right(), result.surfaceNormal);

    DestroyBody(body1);
    DestroyBody(body2);
    DestroyBody(body3);
}

TEST_F(CollisionQueryTest, CastRay_missesTarget_findsNone)
{
    const auto body = CreateBody(g_entity1, g_sphere, JPH::Vec3{1.0f, 1.0f, 1.0f});
    const auto ray = nc::Ray{nc::Vector3{0.0f, 1.0f, 1.0f}, nc::Vector3::Left() * 15.0f};

    auto uut = nc::CollisionQuery();
    const auto result = uut.CastRay(ray);
    EXPECT_EQ(nc::Entity::Null(), result.hit);

    DestroyBody(body);
}

TEST_F(CollisionQueryTest, CastRay_outOfRange_findsNone)
{
    const auto body = CreateBody(g_entity1, g_sphere, JPH::Vec3::sReplicate(10.0f));
    const auto ray = nc::Ray{nc::Vector3::Zero(), nc::Vector3::Splat(9.0f)};

    auto uut = nc::CollisionQuery();
    const auto result = uut.CastRay(ray);
    EXPECT_EQ(nc::Entity::Null(), result.hit);

    DestroyBody(body);
}

TEST_F(CollisionQueryTest, TestShape_multipleTargets_findsOverlapping)
{
    const auto hitBody1 = CreateBody(g_entity1, g_sphere, JPH::Vec3{2.0f, 4.0f, 0.0f});
    const auto hitBody2 = CreateBody(g_entity2, g_sphere, JPH::Vec3{2.0f, 0.0f, 0.0f});
    const auto missedBody = CreateBody(g_entity2, g_sphere, JPH::Vec3{-2.0f, 2.0f, 0.0f});
    const auto sphere = nc::Shape::MakeSphere(2.0f, nc::Vector3{2.0f, 2.0f, 0.0f});

    auto uut = nc::CollisionQuery();
    const auto result = uut.TestShape(sphere);
    ASSERT_EQ(2ull, result.hits.size());

    auto hit1Pos = std::ranges::find(result.hits, g_entity1, &nc::TestShapeHit::hit);
    ASSERT_NE(hit1Pos, result.hits.end());
    const auto& hit1 = *hit1Pos;
    EXPECT_EQ(nc::Vector3(2.0f, 3.5f, 0.0f), hit1.point);
    EXPECT_EQ(nc::Vector3::Up(), hit1.collisionNormal);
    EXPECT_FLOAT_EQ(0.5f, hit1.depth);

    auto hit2Pos = std::ranges::find(result.hits, g_entity2, &nc::TestShapeHit::hit);
    ASSERT_NE(hit2Pos, result.hits.end());
    const auto& hit2 = *hit2Pos;
    EXPECT_EQ(nc::Vector3(2.0f, 0.5f, 0.0f), hit2.point);
    EXPECT_EQ(nc::Vector3::Down(), hit2.collisionNormal);
    EXPECT_FLOAT_EQ(0.5f, hit2.depth);

    DestroyBody(hitBody1);
    DestroyBody(hitBody2);
    DestroyBody(missedBody);
}

TEST_F(CollisionQueryTest, TestShape_missesTarget_findsNone)
{
    const auto missedBody1 = CreateBody(g_entity1, g_sphere, JPH::Vec3::sReplicate(4.0f));
    const auto missedBody2 = CreateBody(g_entity2, g_sphere, JPH::Vec3::sReplicate(-4.0f));
    const auto sphere = nc::Shape::MakeSphere(2.0f, nc::Vector3::Zero());

    auto uut = nc::CollisionQuery();
    const auto result = uut.TestShape(sphere);
    ASSERT_TRUE(result.hits.empty());

    DestroyBody(missedBody1);
    DestroyBody(missedBody2);
}

TEST_F(CollisionQueryTest, TestPoint_multipleTargets_findsContainment)
{
    const auto hitBody1 = CreateBody(g_entity1, g_sphere, JPH::Vec3::sReplicate(0.5f));
    const auto hitBody2 = CreateBody(g_entity2, g_sphere, JPH::Vec3::sReplicate(1.0f));
    const auto missedBody = CreateBody(g_entity2, g_sphere, JPH::Vec3::sReplicate(-1.0f));
    const auto point = nc::Vector3::Splat(0.75f);

    auto uut = nc::CollisionQuery();
    const auto hits = uut.TestPoint(point);
    ASSERT_EQ(2ull, hits.size());
    EXPECT_TRUE(std::ranges::contains(hits, g_entity1));
    EXPECT_TRUE(std::ranges::contains(hits, g_entity2));
    EXPECT_FALSE(std::ranges::contains(hits, g_entity3));

    DestroyBody(hitBody1);
    DestroyBody(hitBody2);
    DestroyBody(missedBody);
}

TEST_F(CollisionQueryTest, TestPoint_outisdieTargets_findsNone)
{
    const auto missedBody1 = CreateBody(g_entity1, g_sphere, JPH::Vec3::sReplicate(0.5f));
    const auto missedBody2 = CreateBody(g_entity2, g_sphere, JPH::Vec3::sReplicate(1.0f));
    const auto missedBody3 = CreateBody(g_entity2, g_sphere, JPH::Vec3::sReplicate(-1.0f));
    const auto point = nc::Vector3::Splat(10.0f);

    auto uut = nc::CollisionQuery();
    const auto hits = uut.TestPoint(point);
    EXPECT_TRUE(hits.empty());

    DestroyBody(missedBody1);
    DestroyBody(missedBody2);
    DestroyBody(missedBody3);
}
