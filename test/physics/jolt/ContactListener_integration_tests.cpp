#include "gtest/gtest.h"
#include "JobSystem_stub.inl"
#include "physics/jolt/ContactListener.h"
#include "physics/jolt/Conversion.h"
#include "physics/jolt/JoltApi.h"
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
        std::vector<nc::physics::OverlappingPair> lastOnTriggerEnter;
        std::vector<nc::physics::OverlappingPair> lastOnExit;
        std::vector<nc::physics::OverlappingPair> lastOnTriggerExit;

        void Step()
        {
            joltApi.physicsSystem.Update(1.0f / 60.0f, 1, &joltApi.tempAllocator, joltApi.jobSystem.get());
            lastOnEnter.clear();
            lastOnTriggerEnter.clear();
            lastOnExit.clear();
            lastOnTriggerExit.clear();
            std::ranges::copy(uut.GetNewCollisions(), std::back_inserter(lastOnEnter));
            std::ranges::copy(uut.GetNewTriggers(), std::back_inserter(lastOnTriggerEnter));
            std::ranges::copy(uut.GetRemovedCollisions(), std::back_inserter(lastOnExit));
            std::ranges::copy(uut.GetRemovedTriggers(), std::back_inserter(lastOnTriggerExit));
            uut.CommitPendingChanges();
        }

        auto CreateBody(nc::Entity entity,
                        nc::BodyType type,
                        const JPH::Vec3& position,
                        const JPH::Vec3& halfExtents,
                        bool isTrigger = false) -> JPH::Body*
        {
            auto settings = JPH::BodyCreationSettings{
                new JPH::BoxShape{halfExtents},
                position,
                JPH::Quat::sIdentity(),
                nc::physics::ToMotionType(type),
                nc::physics::ToObjectLayer(type, isTrigger)
            };

            settings.mUserData = nc::Entity::Hash{}(entity);
            settings.mIsSensor = isTrigger;
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
        nc::BodyType::Static,
        JPH::Vec3::sReplicate(0.0f),
        JPH::Vec3{5.0f, 0.5f, 5.0f}
    );

    auto body2 = CreateBody(
        entity2,
        nc::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(0.5f)
    );

    // expect enter event on start
    Step();
    EXPECT_EQ(1u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());
    const auto& enter = lastOnEnter.at(0);
    EXPECT_TRUE(
        (entity1 == enter.pair.first && entity2 == enter.pair.second) ||
        (entity1 == enter.pair.second && entity2 == enter.pair.first)
    );

    // still colliding, but no events
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    // expect no events on sleep
    interface.DeactivateBody(body2->GetID());
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    // expect no events on wake
    interface.ActivateBody(body2->GetID());
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    // expect exit event when moved apart
    interface.SetPosition(body2->GetID(), JPH::Vec3{0.0f, 2.0f, 0.0f}, JPH::EActivation::Activate);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(1u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());
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
            EXPECT_EQ(0u, lastOnTriggerEnter.size());
            EXPECT_EQ(0u, lastOnTriggerExit.size());
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

TEST_F(ContactListenerTest, Events_triggerVsDynamicAndKinematic_generatesExpectedEvents)
{
    const auto entity1 = nc::Entity{1, 0, 0};
    const auto entity2 = nc::Entity{2, 0, 0};
    const auto entity3 = nc::Entity{3, 0, 0};
    const auto entity4 = nc::Entity{4, 0, 0};
    const auto scale = JPH::Vec3::sReplicate(2.0f);

    auto dynamicTrigger1 = CreateBody(entity1, nc::BodyType::Dynamic, JPH::Vec3{-0.5f, 1.0f, 0.0f}, scale, true);
    auto dynamicTrigger2 = CreateBody(entity2, nc::BodyType::Dynamic, JPH::Vec3{0.5f, 1.0f, 0.0f}, scale, true);
    auto kinematicTrigger = CreateBody(entity3, nc::BodyType::Static, JPH::Vec3{-0.5f, 0.0f, 0.0f}, scale, true);
    auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();


    // expect all triggers to detect hit against dynamic body
    auto dynamic = CreateBody(entity4, nc::BodyType::Dynamic, JPH::Vec3{0.0f, 0.0f, 0.0f}, scale, false);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(3u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    // expect all triggers to exit on move
    interface.SetPosition(dynamic->GetID(), JPH::Vec3{0.0f, 10.0f, 0.0f}, JPH::EActivation::Activate);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(3u, lastOnTriggerExit.size());

    DestroyBody(dynamic->GetID());

    // expect all triggers to detect hit against dynamic body
    auto kinematic = CreateBody(entity4, nc::BodyType::Kinematic, JPH::Vec3{0.0f, 0.0f, 0.0f}, scale, false);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(3u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    // expect all triggers to exit on move
    interface.SetPosition(kinematic->GetID(), JPH::Vec3{0.0f, 10.0f, 0.0f}, JPH::EActivation::Activate);
    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(3u, lastOnTriggerExit.size());

    DestroyBody(kinematic->GetID());
    DestroyBody(dynamicTrigger1->GetID());
    DestroyBody(dynamicTrigger2->GetID());
    DestroyBody(kinematicTrigger->GetID());
}

TEST_F(ContactListenerTest, Events_triggerVsTriggerAndStatic_generatesNoEvents)
{
    const auto entity1 = nc::Entity{1, 0, 0};
    const auto entity2 = nc::Entity{10, 0, 0};
    const auto entity3 = nc::Entity{20, 0, 0};
    const auto staticEntity1 = nc::Entity{30, 0, nc::Entity::Flags::Static};
    const auto staticEntity2 = nc::Entity{40, 0, nc::Entity::Flags::Static};
    const auto scale = JPH::Vec3::sReplicate(2.0f);

    auto dynamicTrigger1 = CreateBody(entity1, nc::BodyType::Dynamic, JPH::Vec3{-0.5f, 1.0f, 0.0f}, scale, true);
    auto dynamicTrigger2 = CreateBody(entity2, nc::BodyType::Dynamic, JPH::Vec3{0.5f, 1.0f, 0.0f}, scale, true);
    auto kinematicTrigger = CreateBody(entity3, nc::BodyType::Static, JPH::Vec3{-0.5f, 0.0f, 0.0f}, scale, true);
    auto staticTrigger = CreateBody(staticEntity1, nc::BodyType::Static, JPH::Vec3{0.5f, 0.0f, 0.0f}, scale, true);
    auto staticNonTrigger = CreateBody(staticEntity2, nc::BodyType::Static, JPH::Vec3{0.0f, 0.0f, 0.0f}, scale, false);

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    DestroyBody(dynamicTrigger1->GetID());
    DestroyBody(dynamicTrigger2->GetID());
    DestroyBody(kinematicTrigger->GetID());
    DestroyBody(staticTrigger->GetID());
    DestroyBody(staticNonTrigger->GetID());
}

TEST_F(ContactListenerTest, Events_staticVsStatic_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::Static};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::Static};
    auto body1 = CreateBody(
        entity1,
        nc::BodyType::Static,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::BodyType::Static,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}

TEST_F(ContactListenerTest, Events_noCollisionEventsVsNoCollisionEvents_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::NoCollisionNotifications};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::NoCollisionNotifications};
    auto body1 = CreateBody(
        entity1,
        nc::BodyType::Dynamic,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}

TEST_F(ContactListenerTest, Events_staticVsNoCollisionEvents_generatesNoEvents)
{
    const auto entity1 = nc::Entity{42, 0, nc::Entity::Flags::Static};
    const auto entity2 = nc::Entity{100, 0, nc::Entity::Flags::NoCollisionNotifications};
    auto body1 = CreateBody(
        entity1,
        nc::BodyType::Static,
        JPH::Vec3{0.0f, 0.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    auto body2 = CreateBody(
        entity2,
        nc::BodyType::Dynamic,
        JPH::Vec3{0.0f, 1.0f, 0.0f},
        JPH::Vec3::sReplicate(2.0f)
    );

    Step();
    EXPECT_EQ(0u, lastOnEnter.size());
    EXPECT_EQ(0u, lastOnExit.size());
    EXPECT_EQ(0u, lastOnTriggerEnter.size());
    EXPECT_EQ(0u, lastOnTriggerExit.size());

    DestroyBody(body1->GetID());
    DestroyBody(body2->GetID());
}
