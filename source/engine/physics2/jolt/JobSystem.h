#pragma once

#include "ncengine/task/TaskFwd.h"

#include <memory>

namespace JPH
{
class JobSystem;
} // namespace JPH

namespace nc::physics
{
auto BuildJobSystem(const task::AsyncDispatcher& dispatcher) -> std::unique_ptr<JPH::JobSystem>;
} // namespace nc::physics
