#include "module/Module.h"
#include "task/Job.h"

namespace nc
{
auto Module::BuildWorkload() -> std::vector<Job>
{
    return {};
}

void Module::Clear() noexcept
{
}
}