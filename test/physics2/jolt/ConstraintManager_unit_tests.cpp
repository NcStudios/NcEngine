#include "JoltApiFixture.inl"
#include "physics2/jolt/ConstraintManager.h"

#include "Jolt/Physics/Constraints/PointConstraint.h"

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

TEST_F(ConstraintManagerTest, AddConstraint_twoBody_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto& actualConstraint = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);

    EXPECT_EQ(g_entity2, actualConstraint.GetConstraintTarget());
    auto& actualInfo = std::get<nc::physics::PointConstraintInfo>(actualConstraint.GetInfo());
    EXPECT_EQ(expectedInfo.point1, actualInfo.point1);
    EXPECT_EQ(expectedInfo.point2, actualInfo.point2);
    EXPECT_EQ(expectedInfo.space, actualInfo.space);

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, AddConstraint_oneBody_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{
        .point1 = nc::Vector3{1.0f, 2.0f, 3.0f},
        .point2 = nc::Vector3{4.0f, 5.0f, 6.0f},
        .space = nc::physics::ConstraintSpace::World
    };

    auto body = CreateBody();
    auto dummyBody = &JPH::Body::sFixedToWorld;
    const auto& actualConstraint = uut.AddConstraint(expectedInfo, g_entity1, body, nc::Entity::Null(), dummyBody);

    const auto constraints = uut.GetConstraints(g_entity1);
    ASSERT_EQ(1ull, constraints.size());

    EXPECT_EQ(nc::Entity::Null(), actualConstraint.GetConstraintTarget());
    const auto& actualInfo = std::get<nc::physics::PointConstraintInfo>(actualConstraint.GetInfo());
    EXPECT_EQ(expectedInfo.point1, actualInfo.point1);
    EXPECT_EQ(expectedInfo.point2, actualInfo.point2);
    EXPECT_EQ(expectedInfo.space, actualInfo.space);

    DestroyBody(body);
}

TEST_F(ConstraintManagerTest, AddConstraint_multipleConstraintsBetweenBodies_succeeds)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();

    const auto& c1 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    EXPECT_EQ(g_entity2, c1.GetConstraintTarget());

    const auto& c2 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    EXPECT_EQ(g_entity2, c2.GetConstraintTarget());

    const auto& c3 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    EXPECT_EQ(g_entity1, c3.GetConstraintTarget());

    const auto& c4 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    EXPECT_EQ(g_entity1, c4.GetConstraintTarget());

    EXPECT_EQ(2ull, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(2ull, uut.GetConstraints(g_entity2).size());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, EnableConstraint_updatesState)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    const auto handle = uut.GetConstraintHandle(constraint.GetId());
    EXPECT_TRUE(constraint.IsEnabled());
    EXPECT_TRUE(handle->GetEnabled());

    uut.EnableConstraint(constraint, false);
    EXPECT_FALSE(constraint.IsEnabled());
    EXPECT_FALSE(handle->GetEnabled());

    uut.EnableConstraint(constraint, true);
    EXPECT_TRUE(constraint.IsEnabled());
    EXPECT_TRUE(handle->GetEnabled());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, EnableConstraint_staleConstraint_throws)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto constraint = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.RemoveConstraint(constraint.GetId());
    EXPECT_THROW(uut.EnableConstraint(constraint, true), std::exception);

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, UpdateConstraint_sameType_updateSettings)
{
    const auto initialInfo = nc::physics::PointConstraintInfo{};
    const auto expectedInfo = nc::physics::PointConstraintInfo{
        .point1 = nc::Vector3{3.0f, 3.0f, 3.0f},
        .point2 = nc::Vector3{4.0f, 4.0f, 4.0f}
    };

    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(initialInfo, g_entity1, body1, g_entity2, body2);

    constraint.GetInfo() = expectedInfo;
    uut.UpdateConstraint(constraint);
    auto handle = uut.GetConstraintHandle(constraint.GetId());
    ASSERT_EQ(JPH::EConstraintSubType::Point, handle->GetSubType());
    auto pointConstraintHandle = static_cast<JPH::PointConstraint*>(handle);

    EXPECT_EQ(expectedInfo.point1, nc::physics::ToVector3(pointConstraintHandle->GetLocalSpacePoint1()));
    EXPECT_EQ(expectedInfo.point2, nc::physics::ToVector3(pointConstraintHandle->GetLocalSpacePoint2()));

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, UpdateConstraint_newType_buildsNewType)
{
    const auto initialInfo = nc::physics::PointConstraintInfo{};
    const auto expectedInfo = nc::physics::FixedConstraintInfo{};

    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(initialInfo, g_entity1, body1, g_entity2, body2);

    constraint.GetInfo() = expectedInfo;
    uut.UpdateConstraint(constraint);
    auto handle = uut.GetConstraintHandle(constraint.GetId());
    EXPECT_EQ(JPH::EConstraintSubType::Fixed, handle->GetSubType());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, UpdateConstraint_disabled_preservesDisabledState)
{
    const auto initialInfo = nc::physics::FixedConstraintInfo{};
    const auto updatedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(initialInfo, g_entity1, body1, g_entity2, body2);
    const auto handle = uut.GetConstraintHandle(constraint.GetId());

    uut.EnableConstraint(constraint, false);
    constraint.GetInfo() = updatedInfo;
    uut.UpdateConstraint(constraint);
    EXPECT_FALSE(constraint.IsEnabled());
    EXPECT_FALSE(handle->GetEnabled());

    uut.UpdateConstraintTarget(constraint, nc::Entity::Null(), &JPH::Body::sFixedToWorld);
    EXPECT_FALSE(constraint.IsEnabled());
    EXPECT_FALSE(handle->GetEnabled());
}

TEST_F(ConstraintManagerTest, UpdateConstraintTarget_attachedToOtherBody_attachToNewBody_updatesMapping)
{
    const auto info = nc::physics::FixedConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto body3 = CreateBody();
    auto& constraint = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2);

    uut.UpdateConstraintTarget(constraint, g_entity3, body3);
    EXPECT_EQ(constraint.GetConstraintTarget(), g_entity3);
    EXPECT_EQ(1u, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(0u, uut.GetConstraints(g_entity2).size());
    EXPECT_EQ(0u, uut.GetConstraints(g_entity3).size());

    DestroyBody(body2);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity2));
    EXPECT_EQ(1u, uut.GetConstraints(g_entity1).size());

    DestroyBody(body3);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity3));
    EXPECT_EQ(0u, uut.GetConstraints(g_entity1).size());

    DestroyBody(body1);
}

TEST_F(ConstraintManagerTest, UpdateConstraintTarget_attachedToOtherBody_attachToWorld_updatesMapping)
{
    const auto info = nc::physics::FixedConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2);

    uut.UpdateConstraintTarget(constraint, nc::Entity::Null(), &JPH::Body::sFixedToWorld);
    EXPECT_FALSE(constraint.GetConstraintTarget().Valid());
    EXPECT_EQ(1u, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(0u, uut.GetConstraints(g_entity2).size());

    DestroyBody(body2);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity2));
    EXPECT_EQ(1u, uut.GetConstraints(g_entity1).size());

    DestroyBody(body1);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity1));
    EXPECT_EQ(0u, uut.GetConstraints(g_entity1).size());
}

TEST_F(ConstraintManagerTest, UpdateConstraintTarget_attachedToWorld_attachToBody_updatesMapping)
{
    const auto info = nc::physics::FixedConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(info, g_entity1, body1, nc::Entity::Null(), &JPH::Body::sFixedToWorld);

    uut.UpdateConstraintTarget(constraint, g_entity2, body2);
    EXPECT_EQ(constraint.GetConstraintTarget(), g_entity2);
    EXPECT_EQ(1u, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(0u, uut.GetConstraints(g_entity2).size());

    DestroyBody(body2);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity2));
    EXPECT_EQ(0u, uut.GetConstraints(g_entity1).size());

    DestroyBody(body1);
    EXPECT_NO_THROW(uut.RemoveConstraints(g_entity1));
}

TEST_F(ConstraintManagerTest, UpdateConstraintTarget_attachToSelf_throws)
{
    const auto info = nc::physics::FixedConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto& constraint = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2);

    EXPECT_THROW(uut.UpdateConstraintTarget(constraint, g_entity1, body1), std::exception);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_twoBody_removesState)
{
    const auto info = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto id = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2).GetId();

    EXPECT_NO_THROW(uut.RemoveConstraint(id));
    EXPECT_TRUE(uut.GetConstraints(g_entity1).empty());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_oneBody_removesState)
{
    const auto info = nc::physics::PointConstraintInfo{};
    auto body = CreateBody();
    const auto id = uut.AddConstraint(info, g_entity1, body, nc::Entity::Null(), &JPH::Body::sFixedToWorld).GetId();

    EXPECT_NO_THROW(uut.RemoveConstraint(id));
    EXPECT_TRUE(uut.GetConstraints(g_entity1).empty());

    DestroyBody(body);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_multipleConstraintsBetweenBodies_preservesOthers)
{
    const auto info = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto toKeepId1 = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2).GetId();
    const auto toRemoveId = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2).GetId();
    const auto toKeepId2 = uut.AddConstraint(info, g_entity1, body1, g_entity2, body2).GetId();

    uut.RemoveConstraint(toRemoveId);
    auto remaining = uut.GetConstraints(g_entity1);
    ASSERT_EQ(2u, remaining.size());
    EXPECT_EQ(toKeepId1, remaining[0].GetId()); // want to preserve order for the editor
    EXPECT_EQ(toKeepId2, remaining[1].GetId());

    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(ConstraintManagerTest, RemoveConstraint_recyclesId)
{
    const auto expectedInfo = nc::physics::PointConstraintInfo{};
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto& toBeDeletedId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2).GetId();
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);

    uut.RemoveConstraint(toBeDeletedId);
    const auto recycledId = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2).GetId();
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
    const auto toBeDeleted = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    const auto toBeDeletedId = toBeDeleted.GetId();
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
    auto constraint2 = uut.AddConstraint(expectedInfo, g_entity1, body1, g_entity2, body2);
    auto constraint3 = uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);
    uut.AddConstraint(expectedInfo, g_entity2, body2, g_entity1, body1);

    uut.RemoveConstraint(constraint2.GetId());
    uut.RemoveConstraint(constraint3.GetId());
    EXPECT_NO_THROW(uut.Clear());
    EXPECT_EQ(0ull, uut.GetConstraints(g_entity1).size());
    EXPECT_EQ(0ull, uut.GetConstraints(g_entity2).size());

    DestroyBody(body1);
    DestroyBody(body2);
}
