#pragma once

#include "Allocator.h"
#include "ContactListener.h"
#include "JobSystem.h"
#include "Layers.h"
#include "ncengine/type/StableAddress.h"

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

namespace physics
{
[[noreturn]] void ThrowJoltUpdateError(JPH::EPhysicsUpdateError error);

struct JoltApi : public StableAddress
{
    ~JoltApi() noexcept;

    static auto Initialize(const config::MemorySettings& memorySettings,
                           const config::PhysicsSettings& physicsSettings,
                           const task::AsyncDispatcher& dispatcher) -> JoltApi;

    void Update(float dt, int steps = 1)
    {
        const auto error = physicsSystem.Update(dt, steps, &tempAllocator, jobSystem.get());
        if (error != JPH::EPhysicsUpdateError::None)
        {
            ThrowJoltUpdateError(error);
        }
    }

    TempAllocator tempAllocator;
    LayerMap layerMap;
    ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    ObjectLayerPairFilter objectLayerPairFilter;
    JPH::PhysicsSystem physicsSystem;
    ContactListener contactListener;
    std::unique_ptr<JPH::JobSystem> jobSystem;

    private:
        JoltApi(const config::MemorySettings& memorySettings,
                const config::PhysicsSettings& physicsSettings,
                const task::AsyncDispatcher& dispatcher);
};
} // namespace physics
} // namespace nc
