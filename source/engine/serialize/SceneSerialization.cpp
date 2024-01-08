#include "ncengine/serialize/SceneSerialization.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/Tag.h"
#include "EntitySerializationUtility.h"

#include "ncutility/BinarySerialization.h"

#include <iostream>
#include <ranges>

namespace
{
void SaveHeader(std::ostream& stream)
{
    constexpr auto header = nc::SceneFragmentHeader{};
    nc::serialize::Serialize(stream, header);
}

void LoadHeader(std::istream& stream)
{
    auto header = nc::SceneFragmentHeader{};
    nc::serialize::Deserialize(stream, header);
    if (header.magicNumber != nc::g_sceneFragmentMagicNumber)
    {
        throw nc::NcError("Unexpected SceneFragment header");
    }

    if (header.version != nc::g_currentSceneFragmentVersion)
    {
        throw nc::NcError("Unexpected SceneFragment version");
    }
}

void SaveAssets(std::ostream& stream, const nc::asset::AssetMap& assets)
{
    nc::serialize::Serialize(stream, assets);
}

void LoadAssets(std::istream& stream, nc::asset::NcAsset& assetModule)
{
    auto assets = nc::asset::AssetMap{};
    nc::serialize::Deserialize(stream, assets);
    assetModule.LoadAssets(assets);
}

void SaveEntities(std::ostream& stream,
                  std::function<bool(nc::Entity)>& entityFilter,
                  nc::SerializationContext& ctx)
{
    const auto entities = nc::BuildFragmentEntityList(ctx.ecs.GetAll<nc::Entity>(), entityFilter, ctx.ecs);
    ctx.entityMap = nc::BuildEntityToFragmentIdMap(entities);
    const auto entityInfos = nc::BuildFragmentEntityInfos(entities, ctx.ecs, ctx.entityMap);
    nc::serialize::Serialize(stream, entityInfos);
}

void LoadEntities(std::istream& stream, nc::DeserializationContext& ctx)
{
    auto entities = std::vector<nc::FragmentEntityInfo>{};
    nc::serialize::Deserialize(stream, entities);
    std::ranges::for_each(entities, [&ctx](auto& entityData)
    {
        nc::RemapEntity(entityData.info.parent, ctx.entityMap);
        ctx.entityMap.emplace(entityData.fragmentId, ctx.ecs.Emplace<nc::Entity>(entityData.info));
    });
}

auto GetSerializablePools(nc::ecs::Ecs ecs) -> std::vector<nc::ecs::ComponentPoolBase*>
{
    auto filter = std::views::filter(ecs.GetComponentPools(), [](auto pool){ return pool->HasSerialize(); });
    return std::vector<nc::ecs::ComponentPoolBase*>{filter.begin(), filter.end()};
}

auto FindPoolById(auto&& pools, size_t poolId) -> nc::ecs::ComponentPoolBase*
{
    auto pos = std::ranges::find_if(pools, [poolId](auto pool) { return pool->Id() == poolId; });
    if (pos == pools.end())
    {
        throw nc::NcError{fmt::format("Expected pool for component id '{}'.", poolId)};
    }

    return *pos;
}

auto FilterEntitiesForPool(const nc::EntityToFragmentIdMap& entityMap,
                           nc::ecs::ComponentPoolBase* pool) -> std::vector<nc::Entity>
{
    auto filter = entityMap
        | std::views::keys
        | std::views::filter([pool](auto&& entity){ return pool->Contains(entity); });

    return std::vector<nc::Entity>(filter.begin(), filter.end());
}

void SaveComponents(std::ostream& stream,
                    nc::ecs::ComponentPoolBase* pool,
                    const nc::SerializationContext& ctx)
{
    const auto poolEntities = FilterEntitiesForPool(ctx.entityMap, pool);
    nc::serialize::Serialize(stream, pool->Id());
    nc::serialize::Serialize(stream, poolEntities.size());
    std::ranges::for_each(poolEntities, [&stream, &pool, &ctx](auto entity)
    {
        nc::serialize::Serialize(stream, ctx.entityMap.at(entity));
        pool->Serialize(stream, entity, ctx);
    });
}

void LoadComponents(std::istream& stream,
                    auto&& pools,
                    const nc::DeserializationContext& ctx)
{
    auto poolId = 0ull;
    auto entityIdCount = 0ull;
    nc::serialize::Deserialize(stream, poolId);
    nc::serialize::Deserialize(stream, entityIdCount);
    auto pool = FindPoolById(pools, poolId);

    std::ranges::for_each(
        std::views::iota(0ull, entityIdCount),
        [&stream, &pool, &ctx](auto)
        {
            auto id = 0u;
            nc::serialize::Deserialize(stream, id);
            pool->Deserialize(stream, ctx.entityMap.at(id), ctx);
        }
    );
}

void SaveComponentPools(std::ostream& stream,
                        nc::SerializationContext& ctx)
{
    auto pools = GetSerializablePools(ctx.ecs);
    nc::serialize::Serialize(stream, pools.size());
    std::ranges::for_each(pools, [&stream, &ctx](auto pool)
    {
        SaveComponents(stream, pool, ctx);
    });
}

void LoadComponentPools(std::istream& stream, nc::DeserializationContext& ctx)
{
    auto poolCount = 0ull;
    nc::serialize::Deserialize(stream, poolCount);
    auto pools = ctx.ecs.GetComponentPools();
    std::ranges::for_each(
        std::views::iota(0ull, poolCount),
        [&stream, &pools, &ctx](auto)
        {
            LoadComponents(stream, pools, ctx);
        });
}
} // anonymous namespace

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

    auto ctx = SerializationContext{.entityMap = {}, .ecs = ecs};
    SaveHeader(stream);
    SaveAssets(stream, assets);
    SaveEntities(stream, entityFilter, ctx);
    SaveComponentPools(stream, ctx);
}

void LoadSceneFragment(std::istream& stream,
                       ecs::Ecs ecs,
                       asset::NcAsset& assetModule)
{
    auto ctx = DeserializationContext{.entityMap = {}, .ecs = ecs};
    LoadHeader(stream);
    LoadAssets(stream, assetModule);
    LoadEntities(stream, ctx);
    LoadComponentPools(stream, ctx);
}
} // namespace nc
