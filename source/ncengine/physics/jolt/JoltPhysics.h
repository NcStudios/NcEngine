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
        const auto error = physicsSystem.Update(dt, steps, &tempAllocator, jobSystem.get());
        if (error != JPH::EPhysicsUpdateError::None)
        {
            ThrowJoltUpdateError(error);
        }
    }

    std::unique_ptr<jolt::JoltApi> api;
    jolt::TempAllocator tempAllocator;
    LayerMap layerMap;
    ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    ObjectLayerPairFilter objectLayerPairFilter;
    JPH::PhysicsSystem physicsSystem;
    ContactListener contactListener;
    std::unique_ptr<JPH::JobSystem> jobSystem;
};
} // namespace physics
} // namespace nc
