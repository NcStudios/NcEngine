#include "JoltApi.h"
#include "ncengine/config/Config.h"
#include "ncutility/NcError.h"

#include "Jolt/Core/Factory.h"
#include "Jolt/RegisterTypes.h"

namespace
{
auto g_factory = std::unique_ptr<JPH::Factory>{};

auto ToJoltSettings(const nc::config::PhysicsSettings& in) -> JPH::PhysicsSettings
{
    auto out = JPH::PhysicsSettings{};
    out.mBaumgarte = in.baumgarteStabilization;
    out.mSpeculativeContactDistance = in.speculativeContactDistance;
    out.mPenetrationSlop = in.penetrationSlop;
    out.mNumVelocitySteps = in.velocitySteps;
    out.mNumPositionSteps = in.positionSteps;
    out.mTimeBeforeSleep = in.timeBeforeSleep;
    out.mPointVelocitySleepThreshold = in.sleepThreshold;
    return out;
}
} // anonymous namespace

namespace nc::physics
{
void ThrowJoltUpdateError(JPH::EPhysicsUpdateError error)
{
    auto messages = std::string{};
    if ((bool)(error & JPH::EPhysicsUpdateError::ManifoldCacheFull))
        messages.append("\n\tManifoldCacheFull");
    if ((bool)(error & JPH::EPhysicsUpdateError::BodyPairCacheFull))
        messages.append("\n\tBodyPairCacheFull");
    if ((bool)(error & JPH::EPhysicsUpdateError::ContactConstraintsFull))
        messages.append("\n\tContactConstraintsFull");

    throw NcError{fmt::format(
        "Physics update failed with '{}'. Errors: {}", std::to_underlying(error), messages
    )};
}

auto JoltApi::Initialize(const config::MemorySettings& memorySettings,
                         const config::PhysicsSettings& physicsSettings,
                         const task::AsyncDispatcher& dispatcher) -> JoltApi
{
    RegisterAllocator();
    g_factory = std::make_unique<JPH::Factory>();
    JPH::Factory::sInstance = g_factory.get();
    JPH::RegisterTypes();
    return JoltApi{memorySettings, physicsSettings, dispatcher};
}

JoltApi::~JoltApi() noexcept
{
    JPH::UnregisterTypes();
    g_factory = nullptr;
    JPH::Factory::sInstance = nullptr;
}

JoltApi::JoltApi(const config::MemorySettings& memorySettings,
                 const config::PhysicsSettings& physicsSettings,
                 const task::AsyncDispatcher& dispatcher)
    : tempAllocator{physicsSettings.tempAllocatorSize},
      contactListener{physicsSystem},
      jobSystem{BuildJobSystem(dispatcher)}

{
    physicsSystem.Init(
        memorySettings.maxRigidBodies,
        0,
        physicsSettings.maxBodyPairs,
        physicsSettings.maxContacts,
        layerMap,
        objectVsBroadphaseFilter,
        objectLayerPairFilter
    );

    physicsSystem.SetPhysicsSettings(ToJoltSettings(physicsSettings));
    physicsSystem.SetContactListener(&contactListener);
}
} // namespace nc::physics
