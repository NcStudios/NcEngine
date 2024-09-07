#pragma once

#include "Allocator.h"
#include "ComponentContext.h"
#include "ContactListener.h"
#include "Layers.h"
#include "ShapeFactory.h"
#include "ncengine/type/StableAddress.h"
#include "ncutility/NcError.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"

namespace nc::physics
{
struct JoltApi : public StableAddress
{
    /** @todo: 690 some of this should move to config */
    static constexpr uint32_t maxBodies = 1024 * 10;
    static constexpr uint32_t numBodyMutexes = 0;
    static constexpr uint32_t maxBodyPairs = 1024 * 10;
    static constexpr uint32_t maxContactConstraints = 1024 * 10 * 4;
    static constexpr uint32_t tempAllocatorSize = 40 * 1024 * 1024;
    static constexpr int maxJobs = JPH::cMaxPhysicsJobs;
    static constexpr int maxBarriers = JPH::cMaxPhysicsBarriers;
    static constexpr int threads = 4;

    ~JoltApi() noexcept;

    static auto Initialize() -> JoltApi;

    void Update(float dt, int steps = 1)
    {
        const auto error = physicsSystem.Update(dt, steps, &tempAllocator, &jobSystem);
        if (error != JPH::EPhysicsUpdateError::None)
        {
            throw NcError{fmt::format("Physics update failed with '{}'", std::to_underlying(error))};
        }
    }

    TempAllocator tempAllocator{tempAllocatorSize};
    JPH::JobSystemThreadPool jobSystem{maxJobs, maxBarriers, threads};
    LayerMap layerMap;
    ObjectVsBroadPhaseLayerFilter objectVsBroadphaseFilter;
    ObjectLayerPairFilter objectLayerPairFilter;
    JPH::PhysicsSystem physicsSystem;
    ShapeFactory shapeFactory;
    ContactListener contactListener;
    std::unique_ptr<ComponentContext> ctx;

    private:
        JoltApi();
};
} // namespace nc::physics
