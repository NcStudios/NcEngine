#include "physics2/jolt/JobSystem.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"

namespace nc::physics
{
auto BuildJobSystem(const task::AsyncDispatcher&) -> std::unique_ptr<JPH::JobSystem>
{
    return std::make_unique<JPH::JobSystemSingleThreaded>(128);
}
} // namespace nc::physics
