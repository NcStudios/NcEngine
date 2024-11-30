#include "ncengine/graphics/GraphicsUtility.h"

#include <array>
#include <vector>

namespace
{
constexpr auto g_materialPassNames = std::array{
    std::string_view{"Shadow"},
    std::string_view{"Toon"},
    std::string_view{"Alpha"},
    std::string_view{"Depth"},
    std::string_view{"Normals"},
    std::string_view{"Outline"}
};

constexpr auto g_materialPassFlags = std::array{
    nc::MaterialPass::Shadow,
    nc::MaterialPass::Toon,
    nc::MaterialPass::Alpha,
    nc::MaterialPass::Depth,
    nc::MaterialPass::Normals,
    nc::MaterialPass::Outline
};

constexpr auto g_postProcessFlags = std::array{
    nc::PostProcessPass::Alpha,
    nc::PostProcessPass::Depth,
    nc::PostProcessPass::Normals,
    nc::PostProcessPass::Outline
};

constexpr auto g_postProcessEffectNames = std::array{
    std::string_view{"MoebiusEffect"}
};

constexpr auto g_postProcessEffectIds = std::array{
    nc::MoebiusEffectId
};

const auto g_postProcessPassFlags = std::array{
    std::vector{
        nc::PostProcessPass::Alpha,
        nc::PostProcessPass::Depth,
        nc::PostProcessPass::Normals,
        nc::PostProcessPass::Outline
    }
};

static_assert(g_materialPassNames.size() == g_materialPassFlags.size());
static_assert(g_postProcessEffectNames.size() == g_postProcessEffectIds.size());
static_assert(g_postProcessPassFlags.size() == g_postProcessEffectIds.size());
} // anonymous namespace

namespace nc
{
auto GetMaterialPassNames() -> std::span<const std::string_view>
{
    return g_materialPassNames;
}

auto GetMaterialPassFlags() -> std::span<const MaterialPass::type>
{
    return g_materialPassFlags;
}

auto GetImplementedMaterialPassFlags() -> std::span<const MaterialPass::type>
{
    return std::span<const MaterialPass::type>{g_materialPassFlags.data() + 1, 1};
}

auto GetPostProcessPassFlags() -> std::span<const PostProcessPass::type>
{
    return g_postProcessFlags;
}

auto GetPostProcessEffectNames() -> std::span<const std::string_view>
{
    return g_postProcessEffectNames;
}

auto GetPostProcessEffectIds() -> std::span<const PostProcessEffectId>
{
    return g_postProcessEffectIds;
}

auto GetPostProcessEffectPassFlags(PostProcessEffectId effectId) -> std::span<const PostProcessPass::type>
{
    return g_postProcessPassFlags.at(effectId);
}
} // namespace nc
