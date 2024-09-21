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

constexpr auto g_constraintTypeNames = std::array{
    "FixedConstraint"sv,
    "PointConstraint"sv
};

constexpr auto g_constraintSpaceNames = std::array{
    "World"sv,
    "Local"sv
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

auto ToString(ConstraintType type) -> std::string_view
{
    return g_constraintTypeNames.at(static_cast<size_t>(type));
}

auto ToConstraintType(std::string_view constraintType) -> ConstraintType
{
    const auto pos = std::ranges::find(g_constraintTypeNames, constraintType);
    if (pos != g_constraintTypeNames.cend())
    {
        const auto index = std::distance(g_constraintTypeNames.cbegin(), pos);
        return static_cast<ConstraintType>(index);
    }

    throw NcError{fmt::format("Unknown ConstraintType '{}'", constraintType)};
}

auto GetConstraintTypeNames() -> std::span<const std::string_view>
{
    return g_constraintTypeNames;
}

auto ToString(ConstraintSpace space) -> std::string_view
{
    return g_constraintSpaceNames.at(static_cast<size_t>(space));
}

auto ToConstraintSpace(std::string_view constraintSpace) -> ConstraintSpace
{
    const auto pos = std::ranges::find(g_constraintSpaceNames, constraintSpace);
    if (pos != g_constraintSpaceNames.cend())
    {
        const auto index = std::distance(g_constraintSpaceNames.cbegin(), pos);
        return static_cast<ConstraintSpace>(index);
    }

    throw NcError{fmt::format("Unknown ConstraintSpace '{}'", constraintSpace)};
}

auto GetConstraintSpaceNames() -> std::span<const std::string_view>
{
    return g_constraintSpaceNames;
}
} // namespace nc::physics
