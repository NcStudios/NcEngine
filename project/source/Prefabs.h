#pragma once
#include "Ecs.h"

namespace project::prefab
{
    enum class Resource : uint32_t;

    void InitializeResources();

    nc::EntityHandle Create(Resource resource, nc::Vector3 position, nc::Quaternion rotation, nc::Vector3 scale, std::string tag);

    inline nc::EntityHandle Create(Resource resource, nc::Vector3 position, nc::Quaternion rotation, nc::Vector3 scale)
    {
        return Create(resource, position, rotation, scale, std::string{});
    }

    inline nc::EntityHandle Create(Resource resource, std::string tag)
    {
        return Create(resource, nc::Vector3::Zero(), nc::Quaternion::Identity(), nc::Vector3::One(), std::move(tag));
    }

    inline nc::EntityHandle Create(Resource resource)
    {
        return Create(resource, nc::Vector3::Zero(), nc::Quaternion::Identity(), nc::Vector3::One(), std::string{});
    }

    enum class Resource : uint32_t
    {
        Boar,
        Cattle,
        Coal,
        CoinOne,
        CoinTwo,
        CoinTen,
        Dog,
        Donkey,
        Dwarf,
        Grain,
        PlayerBoard,
        Ruby,
        Sheep,
        Stable,
        StartingPlayerPiece,
        Stone,
        Vegetable,
        Wood,
        FurnishingTileAdditionalDwelling,
        FurnishingTileCoupleDwelling,
        FurnishingTileDwelling,
        FurnishingTileBeerParlor,
        FurnishingTileBlacksmithingParlor,
        FurnishingTileBlacksmith,
        FurnishingTileBreakfastRoom,
        FurnishingTileBreedingCave,
        FurnishingTileBroomChamber,
        FurnishingTileBuilder,
        FurnishingTileCarpenter,
        FurnishingTileCookingCave,
        FurnishingTileCuddleRoom,
        FurnishingTileDogSchool,
        FurnishingTileFodderChamber,
        FurnishingTileFoodChamber,
        FurnishingTileGuestRoom,
        FurnishingTileHuntingParlor,
        FurnishingTileMainStorage,
        FurnishingTileMilkingParlor,
        FurnishingTileMiner,
        FurnishingTileMiningCave,
        FurnishingTileMixedDwelling,
        FurnishingTileOfficeRoom,
        FurnishingTileOreStorage,
        FurnishingTilePeacefulCave,
        FurnishingTilePrayerChamber,
        FurnishingTileQuarry,
        FurnishingTileRubySupplier,
        FurnishingTileSeam,
        FurnishingTileSimpleDwelling,
        FurnishingTileSimpleDwelling2,
        FurnishingTileSlaughterCave,
        FurnishingTileSparePartStorage,
        FurnishingTileStateParlor,
        FurnishingTileStoneCarver,
        FurnishingTileStoneStorage,
        FurnishingTileStoneSupplier,
        FurnishingTileStubbleRoom,
        FurnishingTileSuppliesStorage,
        FurnishingTileTrader,
        FurnishingTileTreasureChamber,
        FurnishingTileWeaponStorage,
        FurnishingTileWeavingParlor,
        FurnishingTileWoodSupplier,
        FurnishingTileWorkingCave,
        FurnishingTileWorkRoom,
        FurnishingTileWritingChamber
    };
} // end namespace project::prefab