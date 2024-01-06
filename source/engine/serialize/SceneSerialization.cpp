#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/Tag.h"
#include "EntitySerializationUtility.h"

#include "ncutility/BinarySerialization.h"

#include <iostream>
#include <ranges>

namespace nc
{
void SaveSceneFragment(std::ostream& stream,
                       ecs::Ecs ecs,
                       const asset::AssetMap& assets,
                       std::function<bool(Entity)> entityFilter)
{
    static constexpr auto defaultEntityFilter = [](Entity){ return true; };
    if (!entityFilter)
        entityFilter = defaultEntityFilter;

    const auto entities = BuildFragmentEntityList(ecs.GetAll<Entity>(), entityFilter, ecs);
    const auto entityMap = BuildEntityToFragmentIdMap(entities);
    const auto entityInfos = BuildFragmentEntityInfos(entities, ecs, entityMap);
    const auto header = SceneFragmentHeader{};
    serialize::Serialize(stream, header);
    serialize::Serialize(stream, assets);
    serialize::Serialize(stream, entityInfos);
}

void LoadSceneFragment(std::istream& stream,
                       ecs::Ecs ecs,
                       asset::NcAsset& assetModule)
{
    auto header = SceneFragmentHeader{};
    serialize::Deserialize(stream, header);
    if (header.magicNumber != g_sceneFragmentMagicNumber)
    {
        throw NcError("Unexpected SceneFragment header");
    }

    if (header.version != g_currentSceneFragmentVersion)
    {
        throw NcError("Unexpected SceneFragment version");
    }

    {
        auto assets = asset::AssetMap{};
        serialize::Deserialize(stream, assets);
        assetModule.LoadAssets(assets);
    }

    {
        auto entities = std::vector<FragmentEntityInfo>{};
        serialize::Deserialize(stream, entities);
        auto entityMap = FragmentIdToEntityMap{};
        std::ranges::for_each(entities, [&ecs, &entityMap](auto& entityData)
        {
            RemapEntity(entityData.info.parent, entityMap);
            entityMap.emplace(entityData.fragmentId, ecs.Emplace<Entity>(entityData.info));
        });
    }
}
} // namespace nc
