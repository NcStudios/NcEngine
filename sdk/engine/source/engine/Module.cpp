#include "module/Module.h"
#include "module/Job.h"

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