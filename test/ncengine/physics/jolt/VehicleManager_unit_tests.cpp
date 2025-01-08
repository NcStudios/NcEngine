#include "JoltApiFixture.inl"
#include "physics/jolt/VehicleManager.h"
#include "physics/jolt/ConstraintFactory.h"

class VehicleManagerTest : public JoltApiFixture
{
    protected:
        nc::physics::ConstraintFactory constraintFactory;
        nc::physics::VehicleManager uut;

        VehicleManagerTest()
            : constraintFactory{joltApi.physicsSystem},
              uut{joltApi.physicsSystem, constraintFactory, 100u}
        {
        }
};

constexpr auto g_entity1 = nc::Entity{0, 0, 0};
constexpr auto g_entity2 = nc::Entity{1, 0, 0};
constexpr auto g_entity3 = nc::Entity{2, 0, 0};

TEST_F(VehicleManagerTest, AddVehicle_validCall_succeeds)
{
    auto body = CreateBody();
    const auto& actualVehicle = uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    EXPECT_EQ(0, actualVehicle.GetId());
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, AddVehicle_hasVehicle_throws)
{
    auto body = CreateBody();
    uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    EXPECT_THROW(uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body), nc::NcError);
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, RemoveVehicle_hasVehicle_clearsState)
{
    auto body = CreateBody();
    uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    EXPECT_TRUE(uut.RemoveVehicle(g_entity1));
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity1));
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, RemoveVehicle_noVehicle_triviallySucceeds)
{
    auto body = CreateBody();
    EXPECT_FALSE(uut.RemoveVehicle(g_entity1));
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, GetVehicle_returnsExpectedPointer)
{
    auto body = CreateBody();
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity1));
    uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    EXPECT_NE(nullptr, uut.GetVehicle(g_entity1));
    uut.RemoveVehicle(g_entity1);
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity1));
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, GetVehicleConstraint_hasConstraint_returnsPointer)
{
    auto body = CreateBody();
    const auto& vehicle = uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    EXPECT_NE(nullptr, uut.GetVehicleConstraint(vehicle.GetId()));
    DestroyBody(body);
}

TEST_F(VehicleManagerTest, GetVehicleConstraint_noConstraint_throws)
{
    auto body = CreateBody();
    const auto& vehicle = uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body);
    const auto id = vehicle.GetId();

    // out of bounds
    EXPECT_THROW(uut.GetVehicleConstraint(id + 1), nc::NcError);

    // in bounds but nullptr
    uut.RemoveVehicle(g_entity1);
    EXPECT_THROW(uut.GetVehicleConstraint(id), nc::NcError);

    DestroyBody(body);
}

TEST_F(VehicleManagerTest, Clear_triviallySucceeds)
{
    EXPECT_NO_THROW(uut.Clear());
}

TEST_F(VehicleManagerTest, Clear_withNullptrs_succeeds)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    auto body3 = CreateBody();

    uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body1);
    uut.AddVehicle(nc::VehicleInfo{}, g_entity2, *body2);
    uut.AddVehicle(nc::VehicleInfo{}, g_entity3, *body3);

    uut.RemoveVehicle(g_entity1);
    uut.RemoveVehicle(g_entity3);
    EXPECT_NO_THROW(uut.Clear());
    EXPECT_TRUE(uut.GetVehicles().empty());
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity1));
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity2));
    EXPECT_EQ(nullptr, uut.GetVehicle(g_entity3));
    EXPECT_TRUE(joltApi.physicsSystem.GetConstraints().empty());

    DestroyBody(body1);
    DestroyBody(body2);
    DestroyBody(body3);
}

TEST_F(VehicleManagerTest, BeginBatch_doesNotAddToSimulationUntilBatchEnded)
{
    const auto batchIndex = uut.BeginBatch();
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body1);
    uut.AddVehicle(nc::VehicleInfo{}, g_entity2, *body2);

    EXPECT_TRUE(joltApi.physicsSystem.GetConstraints().empty());

    uut.EndBatch(batchIndex);
    EXPECT_EQ(2u, joltApi.physicsSystem.GetConstraints().size());

    uut.RemoveVehicle(g_entity1);
    uut.RemoveVehicle(g_entity2);
    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(VehicleManagerTest, BeginBatch_emptyBatch_succeeds)
{
    const auto batchIndex = uut.BeginBatch();
    EXPECT_NO_THROW(uut.EndBatch(batchIndex));
}

TEST_F(VehicleManagerTest, BeginBatch_nonEmptyFreeList_allocatesSequentiallyFromEnd)
{
    auto body1 = CreateBody();
    auto body2 = CreateBody();
    const auto removedId1 = uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body1).GetId();
    const auto removedId2 = uut.AddVehicle(nc::VehicleInfo{}, g_entity2, *body2).GetId();
    uut.RemoveVehicle(g_entity1);
    uut.RemoveVehicle(g_entity2);

    const auto batchIndex = uut.BeginBatch();
    const auto& batchedId1 = uut.AddVehicle(nc::VehicleInfo{}, g_entity1, *body1).GetId();
    const auto& batchedId2 = uut.AddVehicle(nc::VehicleInfo{}, g_entity2, *body2).GetId();
    EXPECT_TRUE(batchedId1 > removedId1 && batchedId1 > removedId2);
    EXPECT_TRUE(batchedId2 > removedId1 && batchedId2 > removedId2);
    EXPECT_EQ(batchedId1 + 1, batchedId2);
    uut.EndBatch(batchIndex);
    EXPECT_EQ(2, joltApi.physicsSystem.GetConstraints().size());

    uut.RemoveVehicle(g_entity1);
    uut.RemoveVehicle(g_entity2);
    DestroyBody(body1);
    DestroyBody(body2);
}

TEST_F(VehicleManagerTest, BeginBatch_batchInProgress_throws)
{
    uut.BeginBatch();
    EXPECT_THROW(uut.BeginBatch(), nc::NcError);
}

TEST_F(VehicleManagerTest, EndBatchAdd_badIndex_throws)
{
    const auto batchIndex = uut.BeginBatch();
    EXPECT_THROW(uut.EndBatch(batchIndex + 1), nc::NcError);
}

TEST_F(VehicleManagerTest, EndBatchAdd_noBatchInProgress_throws)
{
    EXPECT_THROW(uut.EndBatch(1), nc::NcError);
}

