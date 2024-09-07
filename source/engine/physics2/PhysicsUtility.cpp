#include "ncengine/physics/PhysicsUtility.h"

#include "ncutility/NcError.h"

#include <array>
#include <ranges>

namespace
{
using namespace std::string_view_literals;
constexpr auto g_bodyTypeNames = std::array{
    "Dynamic"sv,
    "Kinematic"sv,
    "Static"sv
};

constexpr auto g_shapeTypeNames = std::array{
    "Box"sv,
    "Sphere"sv,
    "Capsule"sv
};
} // anonymous namespace

namespace nc::physics
{
auto ToString(BodyType type) -> std::string_view
{
    return g_bodyTypeNames.at(static_cast<size_t>(type));
}

auto ToBodyType(std::string_view bodyType) -> BodyType
{
    const auto pos = std::ranges::find(g_bodyTypeNames, bodyType);
    if (pos != g_bodyTypeNames.cend())
    {
        const auto index = std::distance(g_bodyTypeNames.cbegin(), pos);
        return static_cast<BodyType>(index);
    }

    throw NcError{fmt::format("Unknown BodyType '{}'", bodyType)};
}

auto GetBodyTypeNames() -> std::span<const std::string_view>
{
    return g_bodyTypeNames;
}

auto ToString(ShapeType type) -> std::string_view
{
    return g_shapeTypeNames.at(static_cast<size_t>(type));
}

auto ToShapeType(std::string_view shapeType) -> ShapeType
{
    const auto pos = std::ranges::find(g_shapeTypeNames, shapeType);
    if (pos != g_shapeTypeNames.cend())
    {
        const auto index = std::distance(g_shapeTypeNames.cbegin(), pos);
        return static_cast<ShapeType>(index);
    }

    throw NcError{fmt::format("Unknown ShapeType '{}'", shapeType)};
}

auto GetShapeTypeNames() -> std::span<const std::string_view>
{
    return g_shapeTypeNames;
}
} // namespace nc::physics
