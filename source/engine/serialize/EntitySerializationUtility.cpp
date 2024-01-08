#include "EntitySerializationUtility.h"

namespace
{
void AddChildren(nc::Entity parent,
                 nc::ecs::ExplicitEcs<nc::Transform> ecs,
                 std::function<bool(nc::Entity)>& filter,
                 std::vector<nc::Entity>& out)
{
    std::ranges::for_each(
        ecs.Get<nc::Transform>(parent)->Children()
            | std::views::filter(filter),
        [&ecs, &filter, &out](auto&& child)
        {
            out.emplace_back(child);
            AddChildren(child, ecs, filter, out);
        }
    );
}

auto ReconstructEntityInfo(nc::Entity entity,
                           nc::ecs::ExplicitEcs<nc::Transform, nc::Tag> ecs) -> nc::EntityInfo
{
    const auto transform = ecs.Get<nc::Transform>(entity);
    const auto tag = ecs.Get<nc::Tag>(entity);
    return nc::EntityInfo
    {
        .position = transform->LocalPosition(),
        .rotation = transform->LocalRotation(),
        .scale = transform->LocalScale(),
        .parent = transform->Parent(),
        .tag = std::string{tag->Value()},
        .layer = entity.Layer(),
        .flags = entity.Flags()
    };
}
} // anonymous namespace

namespace nc
{
auto BuildFragmentEntityList(std::span<const Entity> in,
                             std::function<bool(Entity)>& filter,
                             ecs::ExplicitEcs<Transform> ecs) -> std::vector<Entity>
{
    // All parents must precede their children, so we want a list of all root nodes that pass the filter.
    auto rootEntities = in | std::views::filter([&filter, &ecs](auto entity)
    {
        return filter(entity) && !ecs.Get<Transform>(entity)->Parent().Valid();
    });

    auto out = std::vector<Entity>{};
    out.reserve(in.size());
    std::ranges::for_each(rootEntities, [&out, &filter, &ecs](auto&& entity)
    {
        out.emplace_back(entity);
        AddChildren(entity, ecs, filter, out);
    });

    return out;
}

auto BuildEntityToFragmentIdMap(std::span<const Entity> entities) -> EntityToFragmentIdMap
{
    auto out = EntityToFragmentIdMap{};
    out.reserve(entities.size());
    std::ranges::for_each(entities, [id = 0u, &out](auto&& entity) mutable
    {
        out.emplace(entity, id++);
    });

    return out;
}

void RemapEntity(Entity& entity, const EntityToFragmentIdMap& map)
{
    if (entity != Entity::Null())
        entity = Entity{map.at(entity), uint8_t{}, uint8_t{}};
}

void RemapEntity(Entity& entity, const FragmentIdToEntityMap& map)
{
    if (entity != Entity::Null())
        entity = Entity{map.at(entity.Index())};
}

auto BuildFragmentEntityInfos(std::span<const Entity> entities,
                              ecs::ExplicitEcs<Transform, Tag> ecs,
                              const EntityToFragmentIdMap& entityMap) -> std::vector<FragmentEntityInfo>
{
    auto out = std::vector<FragmentEntityInfo>{};
    out.reserve(entities.size());
    std::ranges::transform(entities, std::back_inserter(out), [&ecs, &entityMap](auto entity)
    {
        auto info = ReconstructEntityInfo(entity, ecs);
        RemapEntity(info.parent, entityMap);
        return FragmentEntityInfo{entityMap.at(entity), std::move(info)};
    });

    return out;
}

} // namespace nc