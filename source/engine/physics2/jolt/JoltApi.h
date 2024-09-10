#pragma once

#include "Allocator.h"
#include "ComponentContext.h"
#include "ContactListener.h"
#include "Layers.h"
#include "ShapeFactory.h"
#include "ncengine/type/StableAddress.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/JobSystemThreadPool.h"
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
                           const config::PhysicsSettings& physicsSettings) -> JoltApi;

    void Update(float dt, int steps = 1)
    {
        const auto error = physicsSystem.Update(dt, steps, &tempAllocator, &jobSystem);
        if (error != JPH::EPhysicsUpdateError::None)
        {
            ThrowJoltUpdateError(error);
        }
    }

    TempAllocator tempAllocator;
    JPH::JobSystemThreadPool jobSystem; /** @todo 715 implement our own connector to Taskflow */
    LayerMap layerMap;
    ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    ObjectLayerPairFilter objectLayerPairFilter;
    JPH::PhysicsSystem physicsSystem;
    ShapeFactory shapeFactory;
    ContactListener contactListener;
    std::unique_ptr<ComponentContext> ctx;

    private:
        JoltApi(const config::MemorySettings& memorySettings,
                const config::PhysicsSettings& physicsSettings);
};
} // namespace physics
} // namespace nc
