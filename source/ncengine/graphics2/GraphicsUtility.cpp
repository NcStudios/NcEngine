#include "ncengine/graphics/GraphicsUtility.h"

#include "ncutility/NcError.h"

#include <array>
#include <vector>
#include <ranges>

namespace
{
constexpr auto g_materialPassNames = std::array{
    std::string_view{"Shadow"},
    std::string_view{"Toon"},
    std::string_view{"Normals"},
    std::string_view{"Alpha"}
};

constexpr auto g_materialPassFlags = std::array{
    nc::MaterialPassFlag::Shadow,
    nc::MaterialPassFlag::Toon,
    nc::MaterialPassFlag::Normals,
    nc::MaterialPassFlag::Alpha,
};

constexpr auto g_miscPassFlags = std::array{
    nc::MiscPassFlag::Wireframe
};

constexpr auto g_postProcessPassNames = std::array{
    std::string_view{"Wave"},
    std::string_view{"Outline"}
};

constexpr auto g_postProcessPassFlags = std::array{
    nc::PostProcessPassFlag::Wave,
    nc::PostProcessPassFlag::Outline
};

constexpr auto g_postProcessEffectNames = std::array{
    std::string_view{"MoebiusEffect"}
};

constexpr auto g_postProcessEffectIds = std::array{
    nc::MoebiusEffectId
};

const auto g_postProcessEffectPassFlags = std::array{
    std::vector{
        nc::PostProcessPassFlag::Wave,
        nc::PostProcessPassFlag::Outline
    }
};

constexpr auto g_combinedPostProcessEffectPassFlags = std::array{
    nc::MoebiusEffectPassFlags
};

static_assert(g_materialPassNames.size() == g_materialPassFlags.size());
static_assert(g_postProcessPassNames.size() == g_postProcessPassFlags.size());
static_assert(g_postProcessEffectNames.size() == g_postProcessEffectIds.size());
static_assert(g_postProcessEffectPassFlags.size() == g_postProcessEffectIds.size());
static_assert(g_combinedPostProcessEffectPassFlags.size() == g_postProcessEffectIds.size());
} // anonymous namespace

namespace nc
{
auto AdjustDimensionsToAspectRatio(const Vector2& dimensions) -> Vector2
{
    auto width = dimensions.x;
    auto height = dimensions.y;

    if (FloatEqual(height, 0.0f))
    {
        height = 0.00000001f;
    }

    auto currentAspectRatio = width / height;
    if (currentAspectRatio > AspectRatio)
    {
        width = AspectRatio * height;
    }
    else
    {
        height = width / AspectRatio;
    }

    return Vector2{width, height};
}

auto GetMaterialPassNames() -> std::span<const std::string_view>
{
    return g_materialPassNames;
}

auto GetMaterialPassFlags() -> std::span<const MaterialPassFlag::type>
{
    return g_materialPassFlags;
}

auto GetImplementedMaterialPassFlags() -> std::span<const MaterialPassFlag::type>
{
    return std::span<const MaterialPassFlag::type>{g_materialPassFlags.data() + 1, 2};
}

auto GetMiscsPassFlags() -> std::span<const MiscPassFlag::type>
{
    return std::span<const MiscPassFlag::type>{g_miscPassFlags.data(), 1};
}

auto GetPostProcessPassNames() -> std::span<const std::string_view>
{
    return g_postProcessPassNames;
}

auto GetPostProcessPassFlags() -> std::span<const PostProcessPassFlag::type>
{
    return g_postProcessPassFlags;
}

auto GetPostProcessPassName(PostProcessPassFlag::type pass) -> std::string_view
{
    const auto pos = std::ranges::find(g_postProcessPassFlags, pass);
    NC_ASSERT(pos != g_postProcessPassFlags.end(), "Invalid post process pass");
    const auto index = static_cast<size_t>(std::distance(g_postProcessPassFlags.begin(), pos));
    return g_postProcessPassNames.at(index);
}

auto GetPostProcessEffectNames() -> std::span<const std::string_view>
{
    return g_postProcessEffectNames;
}

auto GetPostProcessEffectIds() -> std::span<const PostProcessEffectId>
{
    return g_postProcessEffectIds;
}

auto GetPostProcessEffectPassFlags(PostProcessEffectId effectId) -> std::span<const PostProcessPassFlag::type>
{
    return g_postProcessEffectPassFlags.at(effectId);
}

auto GetCombinedPostProcessEffectPassFlags(PostProcessEffectId effectId) -> PostProcessEffectPassFlags
{
    return g_combinedPostProcessEffectPassFlags.at(effectId);
}
} // namespace nc
