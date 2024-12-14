#include "ncengine/graphics/PostProcess.h"

#include "ncutility/NcError.h"

#include <utility>

namespace nc
{
auto PassHasProperties(PostProcessPassFlag::type pass) -> bool
{
    return pass == PostProcessPassFlag::Outline;
}

auto MakeDefaultPassProperties(PostProcessPassFlag::type pass) -> PostProcessPassProperties
{
    switch (pass)
    {
        case PostProcessPassFlag::Alpha:
        case PostProcessPassFlag::Depth:
        case PostProcessPassFlag::Normals:
            return PostProcessPassProperties{EmptyPassProperties{}};
        case PostProcessPassFlag::Outline:
            return PostProcessPassProperties{OutlinePassProperties{}};
        default:
            // Explicitly enumerate above and fail here so tests can catch if an update to this gets missed.
            NC_ASSERT(false, "Unexpected PostProcessPass");
            std::unreachable();
    }
}
} // namespace nc
