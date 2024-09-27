#include "physics2/jolt/JobSystem.h"

#include "Jolt/Jolt.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"

// Ensure real AsyncDispatcher isn't also linked
#ifdef _MSC_VER
#pragma detect_mismatch("AsyncDispatcher", "Stub")
#endif

namespace nc
{
namespace task
{
class AsyncDispatcher{};
} // namespace task

namespace physics
{
// Use Jolt-provided system for tests to limit dependencies + setup
auto BuildJobSystem(const task::AsyncDispatcher&) -> std::unique_ptr<JPH::JobSystem>
{
    return std::make_unique<JPH::JobSystemSingleThreaded>(128);
}
} // namespace physics
} // namespace nc
