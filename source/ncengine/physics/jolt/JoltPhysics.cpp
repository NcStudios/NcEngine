#include "JoltPhysics.h"
#include "ncengine/physics/PhysicsSnapshot.h"
#include "ncengine/config/Config.h"

#include "ncengine/utility/Log.h"
#include "ncjolt/JoltApi.h"
#include "ncjolt/Profiler.inl"
#include "ncutility/NcError.h"

#include "Jolt/Core/Factory.h"
#include "Jolt/RegisterTypes.h"

namespace
{
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

auto AssertCallback([[maybe_unused]] const char* expression,
                    [[maybe_unused]] const char* message,
                    [[maybe_unused]] const char* file,
                    [[maybe_unused]] unsigned line) -> bool
{
    [[maybe_unused]] constexpr auto subsystem = "Jolt";
    const auto fullMessage = fmt::format(
        "message: {} (expression: {})",
        message ? message : "",
        expression ? expression : ""
    );

    NC_LOG_ERROR_EXT(subsystem, file, line, fullMessage)
    return true;
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

JoltPhysics::JoltPhysics(const config::MemorySettings& memorySettings,
                         const config::PhysicsSettings& physicsSettings,
                         const task::AsyncDispatcher& dispatcher)
    : api{std::make_unique<jolt::JoltApi>(::AssertCallback)},
      tempAllocator{physicsSettings.tempAllocatorSize},
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

JoltPhysics::~JoltPhysics() noexcept = default;

void JoltPhysics::Tick(float dt, uint32_t steps)
{
    while (steps != 0)
    {
        const auto error = physicsSystem.Update(dt, 1, &tempAllocator, jobSystem.get());
        if (error != JPH::EPhysicsUpdateError::None)
        {
            ThrowJoltUpdateError(error);
        }

        --steps;
        ++currentTick;
    }
}

void JoltPhysics::SaveSnapshot(PhysicsSnapshot& snapshot)
{
    snapshot.Save(std::any{&physicsSystem}, currentTick);
}

auto JoltPhysics::RestoreFromSnapshot(PhysicsSnapshot& snapshot) -> bool
{
    const auto restoreTick = snapshot.GetTick();
    NC_ASSERT(restoreTick < currentTick, "Cannot restore to a snapshot newer than the current physics tick.");
    if (snapshot.Restore(std::any{&physicsSystem}))
    {
        currentTick = restoreTick;
        return true;
    }

    return false;
}
} // namespace nc::physics
