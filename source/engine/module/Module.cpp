#include "module/Module.h"
#include "task/Job.h"

namespace nc
{
auto Module::BuildWorkload() -> std::vector<task::Job>
{
    return {};
}

void Module::Clear() noexcept
{
}
}