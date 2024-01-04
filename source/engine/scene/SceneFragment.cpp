#include "ncengine/scene/SceneFragment.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/Tag.h"

#include <iostream>
#include <ranges>



#include "ncutility/BinarySerialization.h"

namespace
{
auto BuildEntityToFragmentIdMap(std::span<const nc::Entity> entities) -> nc::EntityToFragmentIdMap
{
    auto out = nc::EntityToFragmentIdMap{};
    std::ranges::for_each(entities, [id = 0u, &out](auto&& entity) mutable
    {
        out.emplace(entity, id++);
    });

    return out;
}

void RemapEntity(nc::Entity& entity, const nc::EntityToFragmentIdMap& map)
{
    if (entity != nc::Entity::Null())
        entity = nc::Entity{map.at(entity), uint8_t{}, uint8_t{}};
}

void RemapEntity(nc::Entity& entity, const nc::FragmentIdToEntityMap& map)
{
    if (entity != nc::Entity::Null())
        entity = nc::Entity{map.at(entity.Index())};
}

void AddChildren(nc::Entity parent,
                 nc::ecs::ExplicitEcs<nc::Transform> ecs,
                 std::function<bool(nc::Entity)>& filter,
                 std::vector<nc::Entity>& out)
{
    const auto children = ecs.Get<nc::Transform>(parent)->Children();
    std::ranges::for_each(children, [&ecs, &filter, &out](auto&& child)
    {
        if (!filter(child))
            return;

        out.emplace_back(child);
        AddChildren(child, ecs, filter, out);
    });
}

auto BuildFragmentEntityList(std::span<const nc::Entity> in,
                             std::function<bool(nc::Entity)>& filter,
                             nc::ecs::ExplicitEcs<nc::Transform> ecs) -> std::vector<nc::Entity>
{
    // All parents must precede their children, so we want a list of all root nodes that pass the filter.
    auto rootEntities = in | std::views::filter([&filter, &ecs](auto entity)
    {
        return filter(entity) &&
               ecs.Get<nc::Transform>(entity)->Parent() == nc::Entity::Null();
    });

    auto out = std::vector<nc::Entity>{};
    out.reserve(in.size());
    std::ranges::for_each(rootEntities, [&out, &filter, &ecs](auto&& entity)
    {
        out.emplace_back(entity);
        AddChildren(entity, ecs, filter, out);
    });

    return out;
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
auto SaveSceneFragment(ecs::Ecs ecs,
                       const asset::NcAsset& assetModule,
                       std::function<bool(Entity)> entityFilter) -> SceneFragment
{
    static constexpr auto defaultEntityFilter = [](Entity){ return true; };
    if (!entityFilter)
        entityFilter = defaultEntityFilter;

    const auto entities = BuildFragmentEntityList(ecs.GetAll<Entity>(), entityFilter, ecs);
    auto entityMap = BuildEntityToFragmentIdMap(entities);
    auto fragmentInfos = std::vector<FragmentEntityInfo>{};
    fragmentInfos.reserve(entities.size());
    std::ranges::transform(entities, std::back_inserter(fragmentInfos), [&ecs, &entityMap](auto entity)
    {
        auto info = ReconstructEntityInfo(entity, ecs);
        RemapEntity(info.parent, entityMap);
        return FragmentEntityInfo{entityMap.at(entity), std::move(info)};
    });

    return SceneFragment{assetModule.GetLoadedAssets(), std::move(fragmentInfos)};
}

void LoadSceneFragment(SceneFragment& fragment,
                       ecs::Ecs ecs,
                       asset::NcAsset& assetModule)
{
    auto entityMap = FragmentIdToEntityMap{};
    assetModule.LoadAssets(fragment.assets);
    std::ranges::for_each(fragment.entities, [&ecs, &entityMap](auto& entityData)
    {
        RemapEntity(entityData.info.parent, entityMap);
        entityMap.emplace(entityData.fragmentId, ecs.Emplace<Entity>(entityData.info));
    });
}

void SerializeScene(std::ostream& stream,
                    ecs::Ecs ecs,
                    const asset::AssetMap& assets,
                    std::function<bool(Entity)> entityFilter)
{
    static constexpr auto defaultEntityFilter = [](Entity){ return true; };
    if (!entityFilter)
        entityFilter = defaultEntityFilter;

    const auto entities = BuildFragmentEntityList(ecs.GetAll<Entity>(), entityFilter, ecs);
    auto entityMap = BuildEntityToFragmentIdMap(entities);
    auto fragmentInfos = std::vector<FragmentEntityInfo>{};
    fragmentInfos.reserve(entities.size());
    std::ranges::transform(entities, std::back_inserter(fragmentInfos), [&ecs, &entityMap](auto entity)
    {
        auto info = ReconstructEntityInfo(entity, ecs);
        RemapEntity(info.parent, entityMap);
        return FragmentEntityInfo{entityMap.at(entity), std::move(info)};
    });


    const auto header = SceneFragmentHeader{};
    serialize::Serialize(stream, header);
    serialize::Serialize(stream, assets);
    serialize::Serialize(stream, fragmentInfos);
}

void DeserializeScene(std::istream& stream,
                      ecs::Ecs ecs,
                      asset::NcAsset& assetModule)
{
    auto header = SceneFragmentHeader{};
    serialize::Deserialize(stream, header);
    if (header.magicNumber != g_sceneFragmentMagicNumber)
        throw NcError("Unexpected SceneFragment header");

    if (header.version != g_currentSceneFragmentVersion)
        throw NcError("Unexpected SceneFragment version");


    auto assets = asset::AssetMap{};
    serialize::Deserialize(stream, assets);
    assetModule.LoadAssets(assets);


    auto entities = std::vector<FragmentEntityInfo>{};
    serialize::Deserialize(stream, entities);
    auto entityMap = FragmentIdToEntityMap{};
    std::ranges::for_each(entities, [&ecs, &entityMap](auto& entityData)
    {
        RemapEntity(entityData.info.parent, entityMap);
        entityMap.emplace(entityData.fragmentId, ecs.Emplace<Entity>(entityData.info));
    });
}
} // namespace nc
