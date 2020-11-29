#pragma once
#include "Ecs.h"

namespace prefab
{
    void InitializeResources();

    template<class T>
    nc::EntityHandle Create(nc::Vector3 position, nc::Vector3 rotation, nc::Vector3 scale, std::string tag);

    template<class T>
    nc::EntityHandle Create(nc::Vector3 position, nc::Vector3 rotation, nc::Vector3 scale)
    {
        return Create<T>(position, rotation, scale, std::string{});
    }

    template<class T>
    nc::EntityHandle Create(std::string tag)
    {
        return Create<T>(nc::Vector3::Zero(), nc::Vector3::Zero(), nc::Vector3::One(), std::move(tag));
    }

    template<class T>
    nc::EntityHandle Create()
    {
        return Create<T>(nc::Vector3::Zero(), nc::Vector3::Zero(), nc::Vector3::One(), std::string{});
    }

    struct Boar;
    struct Cattle;
    struct Coal;
    struct CoinOne;
    struct CoinTwo;
    struct CoinTen;
    struct Dog;
    struct Donkey;
    struct Dwarf;
    struct Grain;
    struct PlayerBoard;
    struct Ruby;
    struct Sheep;
    struct Stable;
    struct StartingPlayerPiece;
    struct Stone;
    struct Vegetable;
    struct Wood;
    struct FurnishingTileAdditionalDwelling;
    struct FurnishingTileCoupleDwelling;
    struct FurnishingTileDwelling;
    struct FurnishingTileBeerParlor;
    struct FurnishingTileBlacksmithingParlor;
    struct FurnishingTileBlacksmith;
    struct FurnishingTileBreakfastRoom;
    struct FurnishingTileBreedingCave;
    struct FurnishingTileBroomChamber;
    struct FurnishingTileBuilder;
    struct FurnishingTileCarpenter;
    struct FurnishingTileCookingCave;
    struct FurnishingTileCuddleRoom;
    struct FurnishingTileDogSchool;
    struct FurnishingTileFodderChamber;
    struct FurnishingTileFoodChamber;
    struct FurnishingTileGuestRoom;
    struct FurnishingTileHuntingParlor;
    struct FurnishingTileMainStorage;
    struct FurnishingTileMilkingParlor;
    struct FurnishingTileMiner;
    struct FurnishingTileMiningCave;
    struct FurnishingTileMixedDwelling;
    struct FurnishingTileOfficeRoom;
    struct FurnishingTileOreStorage;
    struct FurnishingTilePeacefulCave;
    struct FurnishingTilePrayerChamber;
    struct FurnishingTileQuarry;
    struct FurnishingTileRubySupplier;
    struct FurnishingTileSeam;
    struct FurnishingTileSimpleDwelling;
    struct FurnishingTileSimpleDwelling2;
    struct FurnishingTileSlaughterCave;
    struct FurnishingTileSparePartStorage;
    struct FurnishingTileStateParlor;
    struct FurnishingTileStoneCarver;
    struct FurnishingTileStoneStorage;
    struct FurnishingTileStoneSupplier;
    struct FurnishingTileStubbleRoom;
    struct FurnishingTileSuppliesStorage;
    struct FurnishingTileTrader;
    struct FurnishingTileTreasureChamber;
    struct FurnishingTileWeaponStorage;
    struct FurnishingTileWeavingParlor;
    struct FurnishingTileWoodSupplier;
    struct FurnishingTileWorkingCave;
    struct FurnishingTileWorkRoom;
    struct FurnishingTileWritingChamber;
}