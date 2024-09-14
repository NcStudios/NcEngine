#include "gtest/gtest.h"
#include "ContactListener_stub.inl"
#include "JobSystem_stub.inl"
#include "ncengine/config/Config.h"
#include "physics2/jolt/JoltApi.h"

#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"

class JoltApiFixture : public ::testing::Test
{
    protected:
        nc::physics::JoltApi joltApi;

        JoltApiFixture(uint32_t tempAllocatorSize = 1024 * 1024 * 4,
                       uint32_t maxBodyPairs = 8,
                       uint32_t maxContacts = 4)
            : joltApi{nc::physics::JoltApi::Initialize(
                  nc::config::MemorySettings{},
                  nc::config::PhysicsSettings{
                    .tempAllocatorSize = tempAllocatorSize,
                    .maxBodyPairs = maxBodyPairs,
                    .maxContacts = maxContacts
                  },
                  nc::task::AsyncDispatcher{}
              )}
        {
        }

        auto CreateBody(const JPH::Vec3& position = JPH::Vec3::sZero(),
                        const JPH::Quat& rotation = JPH::Quat::sIdentity()) -> JPH::Body*
        {
            auto bodySettings = JPH::BodyCreationSettings{
                new JPH::BoxShape(JPH::Vec3::sReplicate(0.5f)),
                position,
                rotation,
                JPH::EMotionType::Dynamic,
                JPH::ObjectLayer{0}
            };

            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            auto body = interface.CreateBody(bodySettings);
            interface.AddBody(body->GetID(), JPH::EActivation::Activate);
            return body;
        }

        void DestroyBody(const JPH::Body* body)
        {
            auto& interface = joltApi.physicsSystem.GetBodyInterfaceNoLock();
            const auto id = body->GetID();
            interface.RemoveBody(id);
            interface.DestroyBody(id);
        }
};
