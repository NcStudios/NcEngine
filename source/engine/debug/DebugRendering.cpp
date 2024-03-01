#ifdef NC_DEBUG_RENDERING_ENABLED
#include "ncengine/debug/DebugRendering.h"

#include "ncutility/NcError.h"

#include <optional>
#include <ranges>

namespace
{
constexpr auto g_flags = nc::Entity::Flags::Persistent |
                         nc::Entity::Flags::NoSerialize |
                         nc::Entity::Flags::Internal |
                         nc::Entity::Flags::Static;

auto g_gameState = std::optional<nc::ecs::BasicEcs<nc::graphics::WireframeRenderer>>{};
auto g_bucket = nc::Entity::Null();
auto g_entities = std::array<nc::Entity, static_cast<size_t>(nc::debug::WireframeType::Count)>{};

constexpr auto ToMeshPath(nc::debug::WireframeType type) -> const char*
{
    switch (type)
    {
        case nc::debug::WireframeType::Cube: return nc::asset::CubeMesh;
        case nc::debug::WireframeType::Plane: return nc::asset::PlaneMesh;
        case nc::debug::WireframeType::Sphere: return nc::asset::SphereMesh;
        default: return "";
    }
}
} // anonymous namespace

namespace nc::debug
{
void DebugRendererInitialize(ecs::BasicEcs<graphics::WireframeRenderer> gameState)
{
    NC_ASSERT(!g_bucket.Valid(), "Debug rendering already initialized");
    g_gameState = gameState;
    g_bucket = gameState.Emplace<Entity>(EntityInfo{.tag = "[Debug]", .flags = g_flags});

    constexpr auto names = std::array{"Cube Wireframes", "Plane Wireframes", "Sphere Wireframes"};
    for (auto [entity, name] : std::views::zip(g_entities, names))
    {
        entity = gameState.Emplace<Entity>(EntityInfo{
            .parent = g_bucket,
            .tag = name,
            .flags = g_flags
        });

        gameState.Emplace<graphics::WireframeRenderer>(
            entity,
            Entity::Null(),
            graphics::WireframeSource::Internal,
            MeshView{},
            std::vector<DirectX::XMMATRIX>{}
        );
    }
}

void DebugRendererNewFrame()
{
    NC_ASSERT(g_gameState.has_value(), "Debug rendering not initialized");
    for (auto [i, entity] : std::views::enumerate(g_entities))
    {
        const auto type = static_cast<WireframeType>(i);
        auto& renderer = g_gameState.value().Get<graphics::WireframeRenderer>(entity);
        renderer.mesh = AcquireMeshAsset(ToMeshPath(type));
        renderer.instances.clear();
    }
}

void DebugRendererAddWireframe(WireframeType type, DirectX::FXMMATRIX matrix)
{
    NC_ASSERT(g_gameState.has_value(), "Debug rendering not initialized");
    const auto index = static_cast<size_t>(type);
    const auto entity = g_entities.at(index);
    g_gameState.value().Get<graphics::WireframeRenderer>(entity).instances.push_back(matrix);
}
} // namespace nc::debug
#endif
