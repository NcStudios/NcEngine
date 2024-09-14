#include "JoltApiFixture.inl"
#include "physics2/jolt/ConstraintManager.h"

class ConstraintManagerTest : public JoltApiFixture
{
    protected:
        nc::physics::ConstraintManager uut;

        ConstraintManagerTest()
            : uut{joltApi.physicsSystem, 100u}
        {
        }
};

constexpr auto g_entity1 = nc::Entity{0, 0, 0};
constexpr auto g_entity2 = nc::Entity{42, 0, 0};
constexpr auto g_entity3 = nc::Entity{21, 0, 0};

TEST_F(ConstraintManagerTest, AddGetRemove_twoBody_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto actualId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);

    const auto entity1Views = uut.GetConstraints(g_entity1);
    const auto entity2Views = uut.GetConstraints(g_entity2);
    ASSERT_EQ(1ull, entity1Views.size());
    ASSERT_EQ(0ull, entity2Views.size());

    const auto& actualView = entity1Views[0];
    EXPECT_EQ(g_entity2, actualView.referencedEntity);
    EXPECT_EQ(actualId, actualView.id);

    const auto& actualInfo = std::get<nc::physics::PointConstraintInfo>(actualView.info);
    EXPECT_EQ(expectedInfo.point1, actualInfo.point1);
    EXPECT_EQ(expectedInfo.point2, actualInfo.point2);
    EXPECT_EQ(expectedInfo.space, actualInfo.space);

    EXPECT_NO_THROW(uut.RemoveConstraint(actualId));

    EXPECT_EQ(0ull, uut.GetConstraints(g_entity1).size());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, AddGetRemove_oneBody_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    auto body = CreateBody();
    auto dummyBody = &JPH::Body::sFixedToWorld;
    const auto actualId = uut.AddConstraint(expectedInfo, g_entity1, body, nc::Entity::Null(), dummyBody);

    const auto views = uut.GetConstraints(g_entity1);
    ASSERT_EQ(1ull, views.size());

    const auto& actualView = views[0];
    EXPECT_EQ(nc::Entity::Null(), actualView.referencedEntity);
    EXPECT_EQ(actualId, actualView.id);

    const auto& actualInfo = std::get<nc::physics::PointConstraintInfo>(actualView.info);
    EXPECT_EQ(expectedInfo.point1, actualInfo.point1);
    EXPECT_EQ(expectedInfo.point2, actualInfo.point2);
    EXPECT_EQ(expectedInfo.space, actualInfo.space);

    EXPECT_NO_THROW(uut.RemoveConstraint(actualId));

    EXPECT_EQ(0ull, uut.GetConstraints(g_entity1).size());

    DestroyBody(body);
}

TEST_F(ConstraintManagerTest, AddGetRemove_multipleConstraintsBetweenBodies_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto body1Id1 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    const auto body1Id2 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    const auto body2Id1 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    const auto body2Id2 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);

    ASSERT_EQ(2ull, uut.GetConstraints(g_entity1).size());
    ASSERT_EQ(2ull, uut.GetConstraints(g_entity2).size());

    uut.RemoveConstraint(body2Id2);
    uut.RemoveConstraint(body2Id1);
    uut.RemoveConstraint(body1Id2);
    uut.RemoveConstraint(body1Id1);

    ASSERT_EQ(0ull, uut.GetConstraints(g_entity1).size());
    ASSERT_EQ(0ull, uut.GetConstraints(g_entity2).size());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_recyclesId)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto toBeDeletedId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);

    uut.RemoveConstraint(toBeDeletedId);
    const auto recycledId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    EXPECT_EQ(toBeDeletedId, recycledId);

    uut.Clear();
    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_badId_throws)
{
    EXPECT_THROW(uut.RemoveConstraint(100), std::exception);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_doubleDelete_throws)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto toBeDeletedId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.RemoveConstraint(toBeDeletedId);
    EXPECT_THROW(uut.RemoveConstraint(toBeDeletedId), std::exception);
}

TEST_F(ConstraintManagerTest, RemoveConstraints_noConstraints_succeeds)
{
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity1));
}

TEST_F(ConstraintManagerTest, RemoveConstraints_removesFromSelfAndOthers)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto body3 = CreateBody();
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity3, body3);
    uut.AddConstraint(expectedInfo, g_entity3, body3, g_entity1, body1);

    ASSERT_EQ(2ull, uut.GetConstraints(g_entity1).size());
    ASSERT_EQ(3ull, uut.GetConstraints(g_entity2).size());
    ASSERT_EQ(1ull, uut.GetConstraints(g_entity3).size());

    uut.RemoveConstraints(g_entity1);
    ASSERT_EQ(0ull, uut.GetConstraints(g_entity1).size());
    ASSERT_EQ(1ull, uut.GetConstraints(g_entity2).size());
    ASSERT_EQ(0ull, uut.GetConstraints(g_entity3).size());

    uut.RemoveConstraints(g_entity2);
    ASSERT_EQ(0ull, uut.GetConstraints(g_entity2).size());
    ASSERT_EQ(0ull, uut.GetConstraints(g_entity3).size());

    DestroyBody(body1);
    DestroyBody(body2);
    DestroyBody(body3);
}

TEST_F(ConstraintManagerTest, Clear_triviallySucceeds)
{
    EXPECT_NO_THROW(uut.Clear());
}

TEST_F(ConstraintManagerTest, Clear_withNullptrsInList_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    auto id2 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    auto id3 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);

    uut.RemoveConstraint(id2);
    uut.RemoveConstraint(id3);
    EXPECT_NO_THROW(uut.Clear());
    EXPECT_EQ(0ull, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(0ull, uut.GetConstraints(g_entity2).size());

    DestroyBody(body1);
    DestroyBody(body2);
}
