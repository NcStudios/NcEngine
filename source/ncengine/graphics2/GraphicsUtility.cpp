#include "ncengine/graphics/GraphicsUtility.h"

#include <array>

namespace
{
constexpr auto g_passNames = std::array{
    std::string_view{"Shadow"},
    std::string_view{"Toon"},
    std::string_view{"Alpha"},
    std::string_view{"Depth"},
    std::string_view{"Normals"},
    std::string_view{"Outline"}
};

constexpr auto g_passFlags = std::array{
    nc::MaterialPass::Shadow,
    nc::MaterialPass::Toon,
    nc::MaterialPass::Alpha,
    nc::MaterialPass::Depth,
    nc::MaterialPass::Normals,
    nc::MaterialPass::Outline
};

static_assert(g_passNames.size() == g_passFlags.size());
} // anonymous namespace

namespace nc
{
auto GetMaterialPassNames() -> std::span<const std::string_view>
{
    return g_passNames;
}

auto GetMaterialPassFlags() -> std::span<const MaterialPass::type>
{
    return g_passFlags;
}
} // namespace nc
