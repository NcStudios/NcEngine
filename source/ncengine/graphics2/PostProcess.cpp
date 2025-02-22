#include "ncengine/graphics/PostProcess.h"

#include "ncutility/NcError.h"

#include <utility>

namespace nc
{
auto PassHasProperties(PostProcessPassFlag::type pass) -> bool
{
    return pass == PostProcessPassFlag::Outline || pass == PostProcessPassFlag::Gradient;
}

auto MakeDefaultPassProperties(PostProcessPassFlag::type pass) -> PostProcessPassProperties
{
    switch (pass)
    {
        case PostProcessPassFlag::Outline:
            return PostProcessPassProperties{OutlinePassProperties{}};
        case PostProcessPassFlag::Fxaa:
            return PostProcessPassProperties{};
        case PostProcessPassFlag::Gradient:
            return PostProcessPassProperties{GradientPassProperties{}};
        default:
            // Explicitly enumerate above and fail here so tests can catch if an update to this gets missed.
            NC_ASSERT(false, "Unexpected PostProcessPass");
            std::unreachable();
    }
}
} // namespace nc
