#include "ncengine/graphics/PostProcess.h"

#include "ncutility/NcError.h"

#include <utility>

namespace nc
{
auto PassHasProperties(PostProcessPass::type pass) -> bool
{
    return pass == PostProcessPass::Outline;
}

auto MakeDefaultPassProperties(PostProcessPass::type pass) -> PostProcessPassProperties
{
    switch (pass)
    {
        case PostProcessPass::Alpha:
        case PostProcessPass::Depth:
        case PostProcessPass::Normals:
            return PostProcessPassProperties{EmptyPassProperties{}};
        case PostProcessPass::Outline:
            return PostProcessPassProperties{OutlinePassProperties{}};
        default:
            // Explicitly enumerate above and fail here so tests can catch if an update to this gets missed.
            NC_ASSERT(false, "Unexpected PostProcessPass");
            std::unreachable();
    }
}
} // namespace nc
