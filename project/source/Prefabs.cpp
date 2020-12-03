#include "Prefabs.h"
#include "component/NetworkDispatcher.h"
#include "project/components/GamePiece.h"
#include "graphics/materials/Material.h"
#include "project/source/network/Packet.h"

using namespace nc;

namespace
{
    bool isInitialized = false;
}

namespace project::prefab
{
namespace mesh
{
    auto Boar = graphics::Mesh{};
    auto Cattle = graphics::Mesh{};
    auto Coal = graphics::Mesh{};
    auto Disc = graphics::Mesh{};
    auto Dog = graphics::Mesh{};
    auto Donkey = graphics::Mesh{};
    auto Dwarf = graphics::Mesh{};
    auto Grain = graphics::Mesh{};
    auto PlayerBoard = graphics::Mesh{};
    auto Ruby = graphics::Mesh{};
    auto Sheep = graphics::Mesh{};
    auto Stable = graphics::Mesh{};
    auto StartingPlayerPiece = graphics::Mesh{};
    auto Stone = graphics::Mesh{};
    auto Vegetable = graphics::Mesh{};
    auto Wood = graphics::Mesh{};
    auto FurnishingTile = graphics::Mesh{};
} // end namespace mesh

namespace material
{
    graphics::Material Boar = graphics::Material{};
    graphics::Material Cattle = graphics::Material{};
    graphics::Material Coal = graphics::Material{};
    graphics::Material CoinOne = graphics::Material{};
    graphics::Material CoinTen = graphics::Material{};
    graphics::Material CoinTwo = graphics::Material{};
    graphics::Material Dog = graphics::Material{};
    graphics::Material Donkey = graphics::Material{};
    graphics::Material Dwarf = graphics::Material{};
    graphics::Material Grain = graphics::Material{};
    graphics::Material PlayerBoard = graphics::Material{};
    graphics::Material Ruby = graphics::Material{};
    graphics::Material Sheep = graphics::Material{};
    graphics::Material Stable = graphics::Material{};
    graphics::Material StartingPlayerPiece = graphics::Material{};
    graphics::Material Stone = graphics::Material{};
    graphics::Material Vegetable = graphics::Material{};
    graphics::Material Wood = graphics::Material{};

    graphics::Material FurnishingTileAdditionalDwelling = graphics::Material{};
    graphics::Material FurnishingTileCoupleDwelling = graphics::Material{};
    graphics::Material FurnishingTileDwelling = graphics::Material{};
    graphics::Material FurnishingTileBeerParlor = graphics::Material{};
    graphics::Material FurnishingTileBlacksmithingParlor = graphics::Material{};
    graphics::Material FurnishingTileBlacksmith = graphics::Material{};
    graphics::Material FurnishingTileBreakfastRoom = graphics::Material{};
    graphics::Material FurnishingTileBreedingCave = graphics::Material{};
    graphics::Material FurnishingTileBroomChamber = graphics::Material{};
    graphics::Material FurnishingTileBuilder = graphics::Material{};
    graphics::Material FurnishingTileCarpenter = graphics::Material{};
    graphics::Material FurnishingTileCookingCave = graphics::Material{};
    graphics::Material FurnishingTileCuddleRoom = graphics::Material{};
    graphics::Material FurnishingTileDogSchool = graphics::Material{};
    graphics::Material FurnishingTileFodderChamber = graphics::Material{};
    graphics::Material FurnishingTileFoodChamber = graphics::Material{};
    graphics::Material FurnishingTileGuestRoom = graphics::Material{};
    graphics::Material FurnishingTileHuntingParlor = graphics::Material{};
    graphics::Material FurnishingTileMainStorage = graphics::Material{};
    graphics::Material FurnishingTileMilkingParlor = graphics::Material{};
    graphics::Material FurnishingTileMiner = graphics::Material{};
    graphics::Material FurnishingTileMiningCave = graphics::Material{};
    graphics::Material FurnishingTileMixedDwelling = graphics::Material{};
    graphics::Material FurnishingTileOfficeRoom = graphics::Material{};
    graphics::Material FurnishingTileOreStorage = graphics::Material{};
    graphics::Material FurnishingTilePeacefulCave = graphics::Material{};
    graphics::Material FurnishingTilePrayerChamber = graphics::Material{};
    graphics::Material FurnishingTileQuarry = graphics::Material{};
    graphics::Material FurnishingTileRubySupplier = graphics::Material{};
    graphics::Material FurnishingTileSeam = graphics::Material{};
    graphics::Material FurnishingTileSimpleDwelling = graphics::Material{};
    graphics::Material FurnishingTileSimpleDwelling2 = graphics::Material{};
    graphics::Material FurnishingTileSlaughterCave = graphics::Material{};
    graphics::Material FurnishingTileSparePartStorage = graphics::Material{};
    graphics::Material FurnishingTileStateParlor = graphics::Material{};
    graphics::Material FurnishingTileStoneCarver = graphics::Material{};
    graphics::Material FurnishingTileStoneStorage = graphics::Material{};
    graphics::Material FurnishingTileStoneSupplier = graphics::Material{};
    graphics::Material FurnishingTileStubbleRoom = graphics::Material{};
    graphics::Material FurnishingTileSuppliesStorage = graphics::Material{};
    graphics::Material FurnishingTileTrader = graphics::Material{};
    graphics::Material FurnishingTileTreasureChamber = graphics::Material{};
    graphics::Material FurnishingTileWeaponStorage = graphics::Material{};
    graphics::Material FurnishingTileWeavingParlor = graphics::Material{};
    graphics::Material FurnishingTileWoodSupplier = graphics::Material{};
    graphics::Material FurnishingTileWorkingCave = graphics::Material{};
    graphics::Material FurnishingTileWorkRoom = graphics::Material{};
    graphics::Material FurnishingTileWritingChamber = graphics::Material{};
} // end namespace material

void InitializeResources()
{
    if(isInitialized)
    {
        return;
    }
    isInitialized = true;
    
    mesh::Boar = graphics::Mesh{"project//Models//BoarPiece.fbx"};
    mesh::Cattle = graphics::Mesh{"project//Models//CattlePiece.fbx"};
    mesh::Coal = graphics::Mesh{"project//Models//CoalPiece.fbx"};
    mesh::Disc = graphics::Mesh{"project//Models//Disc.fbx"};
    mesh::Dog = graphics::Mesh{"project//Models//DogPiece.fbx"};
    mesh::Donkey = graphics::Mesh{"project//Models//DonkeyPiece.fbx"};
    mesh::Grain = graphics::Mesh{"project//Models//GrainPiece.fbx"};
    mesh::Ruby = graphics::Mesh{"project//Models//RubyPiece.fbx"};
    mesh::Sheep = graphics::Mesh{"project//Models//SheepPiece.fbx"};
    mesh::Stable = graphics::Mesh{"project//Models//StablePiece.fbx"};
    mesh::StartingPlayerPiece = graphics::Mesh{"project//Models//StartingPlayerPiece.fbx"};
    mesh::Stone = graphics::Mesh{"project//Models//StonePiece.fbx"};
    mesh::Vegetable = graphics::Mesh{"project//Models//VegetablePiece.fbx"};
    mesh::Wood = graphics::Mesh{"project//Models//WoodPiece.fbx"};
    mesh::FurnishingTile = graphics::Mesh{"project/Models/FurnishingTile.fbx"};
    mesh::PlayerBoard = graphics::Mesh{"project/Models/PlayerBoard.fbx"};

    const std::vector<std::string> boarTextures = {"project//Textures//BoarPiece_Material_BaseColor.png", "project//Textures//BoarPiece_Material_Normal.png",  "project//Textures//BoarPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Boar = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, boarTextures};

    const std::vector<std::string> cattleTextures = {"project//Textures//CattlePiece_Material_BaseColor.png", "project//Textures//CattlePiece_Material_Normal.png",  "project//Textures//CattlePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Cattle = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, cattleTextures};

    const std::vector<std::string> coalTextures = {"project//Textures//CoalPiece_Material_BaseColor.png", "project//Textures//CoalPiece_Material_Normal.png",  "project//Textures//CoalPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Coal = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, coalTextures};

    const std::vector<std::string> coinOneTextures = {"project//Textures//OneCoinPiece_Material_BaseColor.png", "project//Textures//CoinPiece_Material_Normal.png",  "project//Textures//CoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::CoinOne = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, coinOneTextures};

    const std::vector<std::string> coinTwoTextures = {"project//Textures//TwoCoinPiece_Material_BaseColor.png", "project//Textures//CoinPiece_Material_Normal.png",  "project//Textures//CoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::CoinTwo = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, coinTwoTextures};

    const std::vector<std::string> coinTenTextures = {"project//Textures//TenCoinPiece_Material_BaseColor.png", "project//Textures//CoinPiece_Material_Normal.png",  "project//Textures//CoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::CoinTen = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, coinTenTextures};

    const std::vector<std::string> dogTextures = {"project//Textures//DogPiece_Material_BaseColor.png", "project//Textures//DogPiece_Material_Normal.png",  "project//Textures//DogPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Dog = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, dogTextures};

    const std::vector<std::string> donkeyTextures = {"project//Textures//DonkeyPiece_Material_BaseColor.png", "project//Textures//DonkeyPiece_Material_Normal.png",  "project//Textures//DonkeyPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Donkey = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, donkeyTextures};

    const std::vector<std::string> dwarfTextures = {"nc//source//graphics//DefaultTexture.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Dwarf = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, dwarfTextures};

    const std::vector<std::string> grainTextures = {"project//Textures//GrainPiece_Material_BaseColor.png", "project//Textures//GrainPiece_Material_Normal.png",  "project//Textures//GrainPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Grain = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, grainTextures};
    
    const std::vector<std::string> rubyTextures = {"project//Textures//RubyPiece_Material_BaseColor.png", "project//Textures//RubyPiece_Material_Normal.png",  "project//Textures//RubyPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Ruby = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, rubyTextures};

    const std::vector<std::string> sheepTextures = {"project//Textures//SheepPiece_Material_BaseColor.png", "project//Textures//SheepPiece_Material_Normal.png",  "project//Textures//SheepPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Sheep = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, sheepTextures};

    const std::vector<std::string> stableTextures = {"nc//source//graphics//DefaultTexture.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Stable = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, stableTextures};

    const std::vector<std::string> startingPlayerPieceTextures = {"project//Textures//StartingPlayerPiece_Material_BaseColor.png", "project//Textures//StartingPlayerPiece_Material_Normal.png",  "project//Textures//StartingPlayerPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::StartingPlayerPiece = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, startingPlayerPieceTextures};

    const std::vector<std::string> stoneTextures = {"project//Textures//StonePiece_Material_BaseColor.png", "project//Textures//StonePiece_Material_Normal.png",  "project//Textures//StonePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Stone = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, stoneTextures};

    const std::vector<std::string> vegetableTextures = {"project//Textures//VegetablePiece_Material_BaseColor.png", "project//Textures//VegetablePiece_Material_Normal.png",  "project//Textures//VegetablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Vegetable = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, vegetableTextures};

    const std::vector<std::string> woodTextures = {"project//Textures//WoodPiece_Material_BaseColor.png", "project//Textures//WoodPiece_Material_Normal.png",  "project//Textures//WoodPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"};
    material::Wood = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, woodTextures};

    const std::vector<std::string> furnishingTileAdditionalDwellingTextures = {"project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileAdditionalDwelling = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileAdditionalDwellingTextures};

    const std::vector<std::string> furnishingTileCoupleDwellingTextures = {"project//Textures//FurnishingTiles//FT_CoupleDwelling_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileCoupleDwelling = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileCoupleDwellingTextures};

    const std::vector<std::string> furnishingTileDwellingTextures = {"project//Textures//FurnishingTiles//FT_Dwelling_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileDwelling = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileDwellingTextures};

    const std::vector<std::string> furnishingTileBeerParlorTextures = {"project//Textures//FurnishingTiles//FT_BeerParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBeerParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBeerParlorTextures};

    const std::vector<std::string> furnishingTileBlacksmithingParlorTextures = {"project//Textures//FurnishingTiles//FT_BlacksmithingParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBlacksmithingParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBlacksmithingParlorTextures};

    const std::vector<std::string> furnishingTileBlacksmithTextures = {"project//Textures//FurnishingTiles//FT_Blacksmith_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBlacksmith = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBlacksmithTextures};

    const std::vector<std::string> furnishingTileBreakfastRoomTextures = {"project//Textures//FurnishingTiles//FT_BreakfastRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBreakfastRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBreakfastRoomTextures};

    const std::vector<std::string> furnishingTileBreedingCaveTextures = {"project//Textures//FurnishingTiles//FT_BreedingCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBreedingCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBreedingCaveTextures};

    const std::vector<std::string> furnishingTileBroomChamberTextures = {"project//Textures//FurnishingTiles//FT_BroomChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBroomChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBroomChamberTextures};

    const std::vector<std::string> furnishingTileBuilderTextures = {"project//Textures//FurnishingTiles//FT_Builder_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileBuilder = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileBuilderTextures};

    const std::vector<std::string> furnishingTileCarpenterTextures = {"project//Textures//FurnishingTiles//FT_Carpenter_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileCarpenter = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileCarpenterTextures};

    const std::vector<std::string> furnishingTileCookingCaveTextures = {"project//Textures//FurnishingTiles//FT_CookingCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileCookingCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileCookingCaveTextures};

    const std::vector<std::string> furnishingTileCuddleRoomTextures = {"project//Textures//FurnishingTiles//FT_CuddleRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileCuddleRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileCuddleRoomTextures};

    const std::vector<std::string> furnishingTileDogSchoolTextures = {"project//Textures//FurnishingTiles//FT_DogSchool_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileDogSchool = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileDogSchoolTextures};

    const std::vector<std::string> furnishingTileFodderChamberTextures = {"project//Textures//FurnishingTiles//FT_FodderChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileFodderChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileFodderChamberTextures};

    const std::vector<std::string> furnishingTileFoodChamberTextures = {"project//Textures//FurnishingTiles//FT_FoodChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileFoodChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileFoodChamberTextures};

    const std::vector<std::string> furnishingTileGuestRoomTextures = {"project//Textures//FurnishingTiles//FT_GuestRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileGuestRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileGuestRoomTextures};

    const std::vector<std::string> furnishingTileHuntingParlorTextures = {"project//Textures//FurnishingTiles//FT_HuntingParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileHuntingParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileHuntingParlorTextures};

    const std::vector<std::string> furnishingTileMainStorageTextures = {"project//Textures//FurnishingTiles//FT_MainStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileMainStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileMainStorageTextures};

    const std::vector<std::string> furnishingTileMilkingParlorTextures = {"project//Textures//FurnishingTiles//FT_MilkingParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileMilkingParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileMilkingParlorTextures};

    const std::vector<std::string> furnishingTileMinerTextures = {"project//Textures//FurnishingTiles//FT_Miner_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileMiner = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileMinerTextures};

    const std::vector<std::string> furnishingTileMiningCaveTextures = {"project//Textures//FurnishingTiles//FT_MiningCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileMiningCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileMiningCaveTextures};

    const std::vector<std::string> furnishingTileMixedDwellingTextures = {"project//Textures//FurnishingTiles//FT_MixedDwelling_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileMixedDwelling = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileMixedDwellingTextures};

    const std::vector<std::string> furnishingTileOfficeRoomTextures = {"project//Textures//FurnishingTiles//FT_OfficeRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileOfficeRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileOfficeRoomTextures};

    const std::vector<std::string> furnishingTileOreStorageTextures = {"project//Textures//FurnishingTiles//FT_OreStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileOreStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileOreStorageTextures};

    const std::vector<std::string> furnishingTilePeacefulCaveTextures = {"project//Textures//FurnishingTiles//FT_PeacefulCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTilePeacefulCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTilePeacefulCaveTextures};

    const std::vector<std::string> furnishingTilePrayerChamberTextures = {"project//Textures//FurnishingTiles//FT_PrayerChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTilePrayerChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTilePrayerChamberTextures};

    const std::vector<std::string> furnishingTileQuarryTextures = {"project//Textures//FurnishingTiles//FT_Quarry_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileQuarry = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileQuarryTextures};

    const std::vector<std::string> furnishingTileRubySupplierTextures = {"project//Textures//FurnishingTiles//FT_RubySupplier_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileRubySupplier = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileRubySupplierTextures};

    const std::vector<std::string> furnishingTileSeamTextures = {"project//Textures//FurnishingTiles//FT_Seam_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSeam = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSeamTextures};

    const std::vector<std::string> furnishingTileSimpleDwellingTextures = {"project//Textures//FurnishingTiles//FT_SimpleDwelling_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSimpleDwelling = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSimpleDwellingTextures};

    const std::vector<std::string> furnishingTileSimpleDwelling2 = {"project//Textures//FurnishingTiles//FT_SimpleDwelling2_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSimpleDwelling2 = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSimpleDwelling2};

    const std::vector<std::string> furnishingTileSlaughterCaveTextures = {"project//Textures//FurnishingTiles//FT_SlaughteringCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSlaughterCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSlaughterCaveTextures};

    const std::vector<std::string> furnishingTileSparePartStorageTextures = {"project//Textures//FurnishingTiles//FT_SparePartStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSparePartStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSparePartStorageTextures};

    const std::vector<std::string> furnishingTileStateParlorTextures = {"project//Textures//FurnishingTiles//FT_StateParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileStateParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileStateParlorTextures};

    const std::vector<std::string> furnishingTileStoneCarverTextures = {"project//Textures//FurnishingTiles//FT_StoneCarver_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileStoneCarver = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileStoneCarverTextures};

    const std::vector<std::string> furnishingTileStoneStorageTextures = {"project//Textures//FurnishingTiles//FT_StoneStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileStoneStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileStoneStorageTextures};

    const std::vector<std::string> furnishingTileStoneSupplierTextures = {"project//Textures//FurnishingTiles//FT_StoneSupplier_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileStoneSupplier = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileStoneSupplierTextures};

    const std::vector<std::string> furnishingTileStubbleRoomTextures = {"project//Textures//FurnishingTiles//FT_StubbleRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileStubbleRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileStubbleRoomTextures};

    const std::vector<std::string> furnishingTileSuppliesStorageTextures = {"project//Textures//FurnishingTiles//FT_SuppliesStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileSuppliesStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileSuppliesStorageTextures};

    const std::vector<std::string> furnishingTileTraderTextures = {"project//Textures//FurnishingTiles//FT_Trader_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileTrader = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileTraderTextures};

    const std::vector<std::string> furnishingTileTreasureChamberTextures = {"project//Textures//FurnishingTiles//FT_TreasureChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileTreasureChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileTreasureChamberTextures};

    const std::vector<std::string> furnishingTileWeaponStorageTextures = {"project//Textures//FurnishingTiles//FT_WeaponStorage_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWeaponStorage = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWeaponStorageTextures};

    const std::vector<std::string> furnishingTileWeavingParlorTextures = {"project//Textures//FurnishingTiles//FT_WeavingParlor_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWeavingParlor = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWeavingParlorTextures};

    const std::vector<std::string> furnishingTileWoodSupplierTextures = {"project//Textures//FurnishingTiles//FT_WoodSupplier_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWoodSupplier = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWoodSupplierTextures};

    const std::vector<std::string> furnishingTileWorkingCaveTextures = {"project//Textures//FurnishingTiles//FT_WorkingCave_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWorkingCave = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWorkingCaveTextures};

    const std::vector<std::string> furnishingTileWorkRoomTextures = {"project//Textures//FurnishingTiles//FT_WorkRoom_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWorkRoom = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWorkRoomTextures};

    const std::vector<std::string> furnishingTileWritingChamberTextures = {"project//Textures//FurnishingTiles//FT_WritingChamber_Material_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::FurnishingTileWritingChamber = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, furnishingTileWritingChamberTextures};

    const std::vector<std::string> playerBoardTextures = {"project//Textures//PlayerBoard_DefaultMaterial_BaseColor.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    material::PlayerBoard = graphics::Material{graphics::TechniqueType::PhongShadingTechnique, playerBoardTextures};
}



template<Resource Resource_t>
EntityHandle Create_(Vector3, Vector3, Vector3, std::string);

template<> EntityHandle Create_<Resource::Boar>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Boar, material::Boar);
    auto transform = Ecs::GetComponent<Transform>(handle);
    auto gamePiece = Ecs::AddComponent<project::GamePiece>(handle, transform);
    auto dispatcher = Ecs::AddComponent<NetworkDispatcher>(handle);
    dispatcher->AddHandler(nc::net::PacketType::TestNetworkDispatcher, std::bind(gamePiece->NetworkDispatchTest, gamePiece, std::placeholders::_1));
    return handle;
}

template<> EntityHandle Create_<project::prefab::Resource::Cattle>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Cattle, material::Cattle);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<project::prefab::Resource::Coal>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Coal, material::Coal);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::CoinOne>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Disc, material::CoinOne);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::CoinTwo>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Disc, material::CoinTwo);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::CoinTen>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Disc, material::CoinTen);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Dog>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Dog, material::Dog);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Donkey>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Donkey, material::Donkey);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Dwarf>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Disc, material::Dwarf);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Grain>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Grain, material::Grain);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Ruby>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Ruby, material::Ruby);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Sheep>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Sheep, material::Sheep);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Stable>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Stable, material::Stable);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::StartingPlayerPiece>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::StartingPlayerPiece, material::StartingPlayerPiece);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Stone>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Stone, material::Stone);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Vegetable>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Vegetable, material::Vegetable);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::Wood>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::Wood, material::Wood);
    auto transform = Ecs::GetComponent<Transform>(handle);
    Ecs::AddComponent<project::GamePiece>(handle, transform);
    return handle;
}

template<> EntityHandle Create_<Resource::PlayerBoard>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::PlayerBoard, material::PlayerBoard);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileAdditionalDwelling>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileAdditionalDwelling);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileCoupleDwelling>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileCoupleDwelling);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileDwelling>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileDwelling);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBeerParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBeerParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBlacksmithingParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBlacksmithingParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBlacksmith>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBlacksmith);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBreakfastRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBreakfastRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBreedingCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBreedingCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBroomChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBroomChamber);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileBuilder>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileBuilder);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileCarpenter>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileCarpenter);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileCookingCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileCookingCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileCuddleRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileCuddleRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileDogSchool>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileDogSchool);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileFodderChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileFodderChamber);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileFoodChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileFoodChamber);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileGuestRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileGuestRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileHuntingParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileHuntingParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileMainStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileMainStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileMilkingParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileMilkingParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileMiner>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileMiner);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileMiningCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileMiningCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileMixedDwelling>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileMixedDwelling);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileOfficeRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileOfficeRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileOreStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileOreStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTilePeacefulCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTilePeacefulCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTilePrayerChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTilePrayerChamber);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileQuarry>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileQuarry);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileRubySupplier>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileRubySupplier);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSeam>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSeam);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSimpleDwelling>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSimpleDwelling);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSimpleDwelling2>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSimpleDwelling2);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSlaughterCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSlaughterCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSparePartStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSparePartStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileStateParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileStateParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileStoneCarver>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileStoneCarver);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileStoneStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileStoneStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileStoneSupplier>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileStoneSupplier);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileStubbleRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileStubbleRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileSuppliesStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileSuppliesStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileTrader>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileTrader);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileTreasureChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileTreasureChamber);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWeaponStorage>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWeaponStorage);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWeavingParlor>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWeavingParlor);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWoodSupplier>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWoodSupplier);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWorkingCave>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWorkingCave);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWorkRoom>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWorkRoom);
    return handle;
}

template<> EntityHandle Create_<Resource::FurnishingTileWritingChamber>(Vector3 position, Vector3 rotation, Vector3 scale, std::string tag)
{
    auto handle = Ecs::CreateEntity(position, rotation, scale, tag);
    Ecs::AddComponent<Renderer>(handle, mesh::FurnishingTile, material::FurnishingTileWritingChamber);
    return handle;
}



using CreateFunc_t = EntityHandle(*)(Vector3, Vector3, Vector3, std::string);

const auto dispatch = std::unordered_map<project::prefab::Resource, CreateFunc_t>
{
    std::pair{Resource::Boar, Create_<Resource::Boar>},
    std::pair{Resource::Cattle, Create_<Resource::Cattle>},
    std::pair{Resource::Coal, Create_<Resource::Coal>},
    std::pair{Resource::CoinOne, Create_<Resource::CoinOne>},
    std::pair{Resource::CoinTwo, Create_<Resource::CoinTwo>},
    std::pair{Resource::CoinTen, Create_<Resource::CoinTen>},
    std::pair{Resource::Dog, Create_<Resource::Dog>},
    std::pair{Resource::Donkey, Create_<Resource::Donkey>},
    std::pair{Resource::Dwarf, Create_<Resource::Dwarf>},
    std::pair{Resource::Grain, Create_<Resource::Grain>},
    std::pair{Resource::PlayerBoard, Create_<Resource::PlayerBoard>},
    std::pair{Resource::Ruby, Create_<Resource::Ruby>},
    std::pair{Resource::Sheep, Create_<Resource::Sheep>},
    std::pair{Resource::Stable, Create_<Resource::Stable>},
    std::pair{Resource::StartingPlayerPiece, Create_<Resource::StartingPlayerPiece>},
    std::pair{Resource::Stone, Create_<Resource::Stone>},
    std::pair{Resource::Vegetable, Create_<Resource::Vegetable>},
    std::pair{Resource::Wood, Create_<Resource::Wood>},
    std::pair{Resource::FurnishingTileAdditionalDwelling, Create_<Resource::FurnishingTileAdditionalDwelling>},
    std::pair{Resource::FurnishingTileCoupleDwelling, Create_<Resource::FurnishingTileCoupleDwelling>},
    std::pair{Resource::FurnishingTileDwelling, Create_<Resource::FurnishingTileDwelling>},
    std::pair{Resource::FurnishingTileBeerParlor, Create_<Resource::FurnishingTileBeerParlor>},
    std::pair{Resource::FurnishingTileBlacksmithingParlor, Create_<Resource::FurnishingTileBlacksmithingParlor>},
    std::pair{Resource::FurnishingTileBlacksmith, Create_<Resource::FurnishingTileBlacksmith>},
    std::pair{Resource::FurnishingTileBreakfastRoom, Create_<Resource::FurnishingTileBreakfastRoom>},
    std::pair{Resource::FurnishingTileBreedingCave, Create_<Resource::FurnishingTileBreedingCave>},
    std::pair{Resource::FurnishingTileBroomChamber, Create_<Resource::FurnishingTileBroomChamber>},
    std::pair{Resource::FurnishingTileBuilder, Create_<Resource::FurnishingTileBuilder>},
    std::pair{Resource::FurnishingTileCarpenter, Create_<Resource::FurnishingTileCarpenter>},
    std::pair{Resource::FurnishingTileCookingCave, Create_<Resource::FurnishingTileCookingCave>},
    std::pair{Resource::FurnishingTileCuddleRoom, Create_<Resource::FurnishingTileCuddleRoom>},
    std::pair{Resource::FurnishingTileDogSchool, Create_<Resource::FurnishingTileDogSchool>},
    std::pair{Resource::FurnishingTileFodderChamber, Create_<Resource::FurnishingTileFodderChamber>},
    std::pair{Resource::FurnishingTileFoodChamber, Create_<Resource::FurnishingTileFoodChamber>},
    std::pair{Resource::FurnishingTileGuestRoom, Create_<Resource::FurnishingTileGuestRoom>},
    std::pair{Resource::FurnishingTileHuntingParlor, Create_<Resource::FurnishingTileHuntingParlor>},
    std::pair{Resource::FurnishingTileMainStorage, Create_<Resource::FurnishingTileMainStorage>},
    std::pair{Resource::FurnishingTileMilkingParlor, Create_<Resource::FurnishingTileMilkingParlor>},
    std::pair{Resource::FurnishingTileMiner, Create_<Resource::FurnishingTileMiner>},
    std::pair{Resource::FurnishingTileMiningCave, Create_<Resource::FurnishingTileMiningCave>},
    std::pair{Resource::FurnishingTileMixedDwelling, Create_<Resource::FurnishingTileMixedDwelling>},
    std::pair{Resource::FurnishingTileOfficeRoom, Create_<Resource::FurnishingTileOfficeRoom>},
    std::pair{Resource::FurnishingTileOreStorage, Create_<Resource::FurnishingTileOreStorage>},
    std::pair{Resource::FurnishingTilePeacefulCave, Create_<Resource::FurnishingTilePeacefulCave>},
    std::pair{Resource::FurnishingTilePrayerChamber, Create_<Resource::FurnishingTilePrayerChamber>},
    std::pair{Resource::FurnishingTileQuarry, Create_<Resource::FurnishingTileQuarry>},
    std::pair{Resource::FurnishingTileRubySupplier, Create_<Resource::FurnishingTileRubySupplier>},
    std::pair{Resource::FurnishingTileSeam, Create_<Resource::FurnishingTileSeam>},
    std::pair{Resource::FurnishingTileSimpleDwelling, Create_<Resource::FurnishingTileSimpleDwelling>},
    std::pair{Resource::FurnishingTileSimpleDwelling2, Create_<Resource::FurnishingTileSimpleDwelling2>},
    std::pair{Resource::FurnishingTileSlaughterCave, Create_<Resource::FurnishingTileSlaughterCave>},
    std::pair{Resource::FurnishingTileSparePartStorage, Create_<Resource::FurnishingTileSparePartStorage>},
    std::pair{Resource::FurnishingTileStateParlor, Create_<Resource::FurnishingTileStateParlor>},
    std::pair{Resource::FurnishingTileStoneCarver, Create_<Resource::FurnishingTileStoneCarver>},
    std::pair{Resource::FurnishingTileStoneStorage, Create_<Resource::FurnishingTileStoneStorage>},
    std::pair{Resource::FurnishingTileStoneSupplier, Create_<Resource::FurnishingTileStoneSupplier>},
    std::pair{Resource::FurnishingTileStubbleRoom, Create_<Resource::FurnishingTileStubbleRoom>},
    std::pair{Resource::FurnishingTileSuppliesStorage, Create_<Resource::FurnishingTileSuppliesStorage>},
    std::pair{Resource::FurnishingTileTrader, Create_<Resource::FurnishingTileTrader>},
    std::pair{Resource::FurnishingTileTreasureChamber, Create_<Resource::FurnishingTileTreasureChamber>},
    std::pair{Resource::FurnishingTileWeaponStorage, Create_<Resource::FurnishingTileWeaponStorage>},
    std::pair{Resource::FurnishingTileWeavingParlor, Create_<Resource::FurnishingTileWeavingParlor>},
    std::pair{Resource::FurnishingTileWoodSupplier, Create_<Resource::FurnishingTileWoodSupplier>},
    std::pair{Resource::FurnishingTileWorkingCave, Create_<Resource::FurnishingTileWorkingCave>},
    std::pair{Resource::FurnishingTileWorkRoom, Create_<Resource::FurnishingTileWorkRoom>},
    std::pair{Resource::FurnishingTileWritingChamber, Create_<Resource::FurnishingTileWritingChamber>}
};

nc::EntityHandle Create(Resource resource, nc::Vector3 position, nc::Vector3 rotation, nc::Vector3 scale, std::string tag)
{
    return dispatch.at(resource)(position, rotation, scale, std::move(tag));
}
} // end namespace project::prefab