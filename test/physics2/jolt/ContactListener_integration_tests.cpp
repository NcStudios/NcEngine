#include "gtest/gtest.h"
#include "JobSystem_stub.inl"
#include "physics2/jolt/ContactListener.h"
#include "physics2/jolt/Conversion.h"
#include "physics2/jolt/JoltApi.h"
#include "ncengine/config/Config.h"
#include "ncengine/physics/RigidBody.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"

#include <ranges>

class ContactListenerTest : public ::testing::Test
{
    protected:
        ContactListenerTest()
            : joltApi{nc::physics::JoltApi::Initialize(
                  nc::config::MemorySettings{},
                  nc::config::PhysicsSettings{
                    .tempAllocatorSize = 1024 * 1024 * 4,
                    .maxBodyPairs = 16,
                    .maxContacts = 8
                  },
                  nc::task::AsyncDispatcher{}
              )},
              uut{joltApi.contactListener}
        {
        }

    public:
        nc::physics::JoltApi joltApi;
        nc::physics::ContactListener& uut;
        std::vector<nc::physics::CollisionPair> lastOnEnter;
        std::vector<nc::physics::OverlappingPair> lastOnExit;

        void Step()
        {
            joltApi.physicsSystem.Update(1.0f / 60.0f, 1, &joltApi.tempAllocator, joltApi.jobSystem.get());
            lastOnEnter.clear();
            lastOnExit.clear();
            std::ranges::copy(uut.GetAdded(), std::back_inserter(lastOnEnter));
            std::ranges::copy(uut.GetRemoved(), std::back_inserter(lastOnExit));
            uut.CommitPendingChanges();
        }

        auto CreateBody(nc::Entity entity,
                        nc::physics::BodyType type,
                        const JPH::Vec3& position,
                        const JPH::Vec3& halfExtents) -> JPH::Body*
        {
            auto settings = JPH::BodyCreationSettings{
                new JPH::BoxShape{halfExtents},
                position,
                JPH::Quat::sIdentity(),
                nc::physics::ToMotionType(type),
                nc::physics::ToObjectLayer(type)
            };

            settings.mUserData = nc::Entity::Hash{}(entity);
            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            auto body = interface.CreateBody(settings);
            interface.AddBody(body->GetID(), JPH::EActivation::Activate);
            return body;
        }

        void DestroyBody(JPH::BodyID id)
        {
            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            interface.RemoveBody(id);
            interface.DestroyBody(id);
        }
};

TEST_F(ContactListenerTest, EmptyFrame_succeeds)
{
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
}

TEST_F(ContactListenerTest, InteractingObjects_generatesExpectedEvents)
{
    auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();

    const auto entity1 = nc::Entity{42, 0, 0};
    const auto entity2 = nc::Entity{100, 0, 0};
    auto body1 = CreateBody(
        entity1,
        nc::physics::BodyType::Static,
        JPH::Vec3::sReplicate(0.0f),
        JPH::Vec3{5.0f, 0.5f, 5.0f}
    );

    auto body2 = CreateBody(
        entity2,
        nc::physics::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(0.5f)
    );

    // expect enter event on start
    Step();
    EXPECT_EQ(1u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    const auto& enter = lastOnEnter.at(0);
    EXPECT_TRUE(
        (entity1 == enter.pair.first && entity2 == enter.pair.second) ||
        (entity1 == enter.pair.second && entity2 == enter.pair.first)
    );

    // still colliding, but no events
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    // expect no events on sleep
    interface.DeactivateBody(body2->GetID());
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    // expect no events on wake
    interface.ActivateBody(body2->GetID());
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    // expect exit event when moved apart
    interface.SetPosition(body2->GetID(), JPH::Vec3{0.0f, 2.0f, 0.0f}, JPH::EActivation::Activate);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(1u, lastOnExit.size());
    const auto& exit = lastOnExit.at(0);
    EXPECT_TRUE(
        (entity1 == exit.first && entity2 == exit.second) ||
        (entity1 == exit.second && entity2 == exit.first)
    );

    // expect enter event to be retriggered in near future
    auto frames = 0;
    while (true)
    {
        if (++frames > 60)
        {
            ADD_FAILURE() << "Objects did no collide within frame limit";
            break;
        }

        Step();
        EXPECT_EQ(0u, lastOnExit.size());
        if (lastOnEnter.size() > 0)
        {
            EXPECT_EQ(1u, lastOnEnter.size());
            const auto& reenter = lastOnEnter.at(0);
            EXPECT_TRUE(
                (entity1 == reenter.pair.first && entity2 == reenter.pair.second) ||
                (entity1 == reenter.pair.second && entity2 == reenter.pair.first)
            );

            break;
        }
    }

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}

TEST_F(ContactListenerTest, Events_staticVsStatic_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::Static};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::Static};
    auto body1 = CreateBody(
        entity1,
        nc::physics::BodyType::Static,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::physics::BodyType::Static,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}

TEST_F(ContactListenerTest, Events_noCollisionEventsVsNoCollisionEvents_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::NoCollisionNotifications};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::NoCollisionNotifications};
    auto body1 = CreateBody(
        entity1,
        nc::physics::BodyType::Dynamic,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::physics::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}

TEST_F(ContactListenerTest, Events_staticVsNoCollisionEvents_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::Static};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::NoCollisionNotifications};
    auto body1 = CreateBody(
        entity1,
        nc::physics::BodyType::Static,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::physics::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}
