#pragma once

#include "ContactListener.h"
#include "JobSystem.h"
#include "Layers.h"

#include "ncengine/type/StableAddress.h"
#include "ncjolt/Allocator.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"

#include <memory>



#include "ncengine/physics/NcPhysics.h"


namespace nc
{
namespace config
{
struct MemorySettings;
struct PhysicsSettings;
} // namespace config

namespace jolt
{
class JoltApi;
} // namespace jolt

namespace physics
{
[[noreturn]] void ThrowJoltUpdateError(JPH::EPhysicsUpdateError error);

struct JoltPhysics : public StableAddress
{
    JoltPhysics(const config::MemorySettings& memorySettings,
                const config::PhysicsSettings& physicsSettings,
                const task::AsyncDispatcher& dispatcher);

    ~JoltPhysics() noexcept;

    void Update(float dt, int steps = 1)
    {
        currentFrame += steps;
        const auto error = physicsSystem.Update(dt, steps, &tempAllocator, jobSystem.get());
        if (error != JPH::EPhysicsUpdateError::None)
        {
            ThrowJoltUpdateError(error);
        }
    }

    void SaveSnapshot(PhysicsSnapshot& snapshot);
    void RestoreFromSnapshot(PhysicsSnapshot& snapshot);

    std::unique_ptr<jolt::JoltApi> api;
    jolt::TempAllocator tempAllocator;
    LayerMap layerMap;
    ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    ObjectLayerPairFilter objectLayerPairFilter;
    JPH::PhysicsSystem physicsSystem;
    ContactListener contactListener;
    std::unique_ptr<JPH::JobSystem> jobSystem;

    size_t currentFrame = 0ull;
};
} // namespace physics
} // namespace nc
