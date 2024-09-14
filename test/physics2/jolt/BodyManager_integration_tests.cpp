#include "JoltApiFixture.inl"
#include "physics2/jolt/BodyManager.h"
#include "physics2/jolt/ConstraintManager.h"
#include "physics2/jolt/ShapeFactory.h"

class BodyManagerTest : public JoltApiFixture
{
    protected:
        static constexpr auto maxEntities = 10u;
        nc::ecs::ComponentPool<nc::Transform> transformPool;
        nc::ecs::ComponentPool<nc::physics::RigidBody> rigidBodyPool;
        nc::physics::ShapeFactory shapeFactory;
        nc::physics::ConstraintManager constraintManager;
        nc::physics::BodyManager uut;

        BodyManagerTest()
            : transformPool{maxEntities, nc::ComponentHandler<nc::Transform>{}},
              rigidBodyPool{maxEntities, nc::ComponentHandler<nc::physics::RigidBody>{}},
              constraintManager{joltApi.physicsSystem, maxEntities},
              uut{
                  transformPool,
                  rigidBodyPool,
                  maxEntities,
                  joltApi.physicsSystem,
                  shapeFactory,
                  constraintManager
              }
        {
        }

        auto AddRigidBody(nc::Entity entity) -> nc::physics::RigidBody&
        {
            transformPool.Emplace(entity, nc::Vector3::Zero(), nc::Quaternion::Identity(), nc::Vector3::One());
            return rigidBodyPool.Emplace(entity);
        }

        auto RemoveRigidBody(nc::Entity entity)
        {
            transformPool.Remove(entity);
            rigidBodyPool.Remove(entity);
        }

        auto GetBodyInterface() -> JPH::BodyInterface&
        {
            return joltApi.physicsSystem.GetBodyInterfaceNoLock();
        }

        auto GetBodyId(const nc::physics::RigidBody& rigidBody) -> JPH::BodyID
        {
            if (!rigidBody.IsInitialized())
            {
                ADD_FAILURE() << "RigidBody not initialized";
                return JPH::BodyID{JPH::BodyID::cInvalidBodyID};
            }

            const auto apiBody = reinterpret_cast<JPH::Body*>(rigidBody.GetHandle());
            return apiBody->GetID();
        }
};

constexpr auto g_entity1 = nc::Entity{1, 0, 0};
constexpr auto g_entity2 = nc::Entity{2, 0, 0};
constexpr auto g_entity3 = nc::Entity{3, 0, 0};

TEST_F(BodyManagerTest, AddBody_addsBodyToSimulation)
{
    auto& body = AddRigidBody(g_entity1);
    ASSERT_TRUE(body.IsInitialized());
    const auto bodyId = GetBodyId(body);
    auto& bodyInterface = GetBodyInterface();
    EXPECT_TRUE(bodyInterface.IsAdded(bodyId));
    EXPECT_TRUE(bodyInterface.IsActive(bodyId));
    RemoveRigidBody(g_entity1);
}

TEST_F(BodyManagerTest, AddBody_initializesFromOnAdd)
{
    auto isInitializedBefore = false;
    auto isInitializedAfter = false;

    [[maybe_unused]] auto _1 = rigidBodyPool.OnAdd().Connect(
        [&isInitializedBefore](auto& rb)
        {
            isInitializedBefore = rb.IsInitialized();
        },
        nc::SignalPriority::Highest
    );

    [[maybe_unused]] auto _2 = rigidBodyPool.OnAdd().Connect(
        [&isInitializedAfter](auto& rb)
        {
            isInitializedAfter = rb.IsInitialized();
        },
        nc::SignalPriority::Lowest
    );

    AddRigidBody(g_entity1);
    EXPECT_FALSE(isInitializedBefore);
    EXPECT_TRUE(isInitializedAfter);
    RemoveRigidBody(g_entity1);
}

TEST_F(BodyManagerTest, AddBody_doubleCall_throws)
{
    auto& body = AddRigidBody(g_entity1);
    EXPECT_THROW(uut.AddBody(body), std::exception);
    RemoveRigidBody(g_entity1);
}

TEST_F(BodyManagerTest, RemoveBody_calledFromOnRemove)
{
    auto isAddedBefore = false;
    auto isAddedAfter = false;

    auto& rigidBody = AddRigidBody(g_entity1);
    const auto bodyId = GetBodyId(rigidBody);
    auto& interface = GetBodyInterface();

    [[maybe_unused]] auto _1 = rigidBodyPool.OnRemove().Connect(
        [&isAddedBefore, &interface, bodyId](auto)
        {
            isAddedBefore = interface.IsAdded(bodyId);
        },
        nc::SignalPriority::Highest
    );

    [[maybe_unused]] auto _2 = rigidBodyPool.OnRemove().Connect(
        [&isAddedAfter, &interface, bodyId](auto)
        {
            isAddedAfter = interface.IsAdded(bodyId);
        },
        nc::SignalPriority::Lowest
    );

    RemoveRigidBody(g_entity1);
    EXPECT_TRUE(isAddedBefore);
    EXPECT_FALSE(isAddedAfter);
}

TEST_F(BodyManagerTest, RemoveBody_deferredCleanupEnabled_skipsRemoval)
{
    auto& rigidBody = AddRigidBody(g_entity1);
    const auto bodyId = GetBodyId(rigidBody);
    uut.DeferCleanup(true);
    uut.RemoveBody(g_entity1);
    auto& bodyInterface = GetBodyInterface();
    EXPECT_TRUE(bodyInterface.IsAdded(bodyId));

    uut.DeferCleanup(false);
    RemoveRigidBody(g_entity1);
}

TEST_F(BodyManagerTest, RemoveBody_badEntity_throws)
{
    EXPECT_THROW(uut.RemoveBody(g_entity1), std::exception);
}

TEST_F(BodyManagerTest, RemoveBody_doubleCall_throws)
{
    AddRigidBody(g_entity1);
    RemoveRigidBody(g_entity1);
    EXPECT_THROW(uut.RemoveBody(g_entity1), std::exception);
}

TEST_F(BodyManagerTest, Clear_empty_succeeds)
{
    EXPECT_NO_THROW(uut.Clear());
}

TEST_F(BodyManagerTest, Clear_hasBodies_removesBodiesFromSimulation)
{
    const auto id1 = GetBodyId(AddRigidBody(g_entity1));
    const auto id2 = GetBodyId(AddRigidBody(g_entity2));
    const auto id3 = GetBodyId(AddRigidBody(g_entity3));
    uut.Clear();

    auto& bodyInterface = GetBodyInterface();
    EXPECT_FALSE(bodyInterface.IsAdded(id1));
    EXPECT_FALSE(bodyInterface.IsAdded(id2));
    EXPECT_FALSE(bodyInterface.IsAdded(id3));
}

TEST_F(BodyManagerTest, SampleWorkflow)
{
    auto& physicsSystem = joltApi.physicsSystem;
    auto& bodyInterface = GetBodyInterface();

    auto id1 = GetBodyId(AddRigidBody(g_entity1));
    auto id2 = GetBodyId(AddRigidBody(g_entity2));
    auto id3 = GetBodyId(AddRigidBody(g_entity3));

    EXPECT_EQ(3u, physicsSystem.GetNumBodies());
    EXPECT_TRUE(bodyInterface.IsAdded(id1));
    EXPECT_TRUE(bodyInterface.IsAdded(id2));
    EXPECT_TRUE(bodyInterface.IsAdded(id3));

    RemoveRigidBody(g_entity2);
    EXPECT_EQ(2u, physicsSystem.GetNumBodies());
    EXPECT_FALSE(bodyInterface.IsAdded(id2));

    id2 = GetBodyId(AddRigidBody(g_entity2));
    EXPECT_EQ(3u, physicsSystem.GetNumBodies());
    EXPECT_TRUE(bodyInterface.IsAdded(id2));

    uut.Clear();
    uut.DeferCleanup(true);
    const auto removed = std::vector<nc::Entity>{g_entity1, g_entity2, g_entity3};
    transformPool.CommitStagedComponents(removed);
    rigidBodyPool.CommitStagedComponents(removed);
    transformPool.ClearNonPersistent();
    rigidBodyPool.ClearNonPersistent();

    EXPECT_EQ(0u, physicsSystem.GetNumBodies());
    EXPECT_FALSE(bodyInterface.IsAdded(id1));
    EXPECT_FALSE(bodyInterface.IsAdded(id2));
    EXPECT_FALSE(bodyInterface.IsAdded(id3));

    uut.DeferCleanup(false);
    id1 = GetBodyId(AddRigidBody(g_entity1));
    RemoveRigidBody(g_entity1);
    EXPECT_EQ(0u, physicsSystem.GetNumBodies());
    EXPECT_FALSE(bodyInterface.IsAdded(id1));
}
