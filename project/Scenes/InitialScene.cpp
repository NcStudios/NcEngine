#include "InitialScene.h"
#include "ECS.h"
#include "NcCamera.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "CamController.h"
#include "DebugUtils.h"
#include "source/Prefabs.h"
#include "GamePiece.h"
#include "ClickHandler.h"

using namespace nc;
using namespace project;

void InitialScene::Load()
{
    prefab::InitializeResources();

    // Light
    auto lvHandle = ECS::CreateEntity({-33.9f, 10.3f, -2.4f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    ECS::AddComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = ECS::CreateEntity({0.0f, 5.0f, 0.0f}, {1.3f, 0.0f, 0.0f}, Vector3::Zero(), "Main Camera");
    auto camComponentPtr = ECS::AddComponent<Camera>(camHandle);
    camera::NcRegisterMainCamera(camComponentPtr);
    ECS::AddComponent<CamController>(camHandle);
    ECS::AddComponent<ClickHandler>(camHandle);
    
    // Debug Controller
    auto debugHandle = ECS::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
    ECS::AddComponent<SceneReset>(debugHandle);
    ECS::AddComponent<Timer>(debugHandle);

    const auto scaleFactor = 2;

    // Table
    auto tableMaterial = graphics::PBRMaterial{{"project//Textures//DiningRoomTable_Material_BaseColor.png", "project//Textures//DiningRoomTable_Material_Normal.png",  "project//Textures//DiningRoomTable_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto tableHandle = ECS::CreateEntity({2.0f  * scaleFactor, -0.4f, 1.5f * scaleFactor}, {1.5708f, 0.0f, 1.5708f}, Vector3::One() * 7.5, "Table Piece");
    auto tableMesh = graphics::Mesh{"project//Models//DiningRoomTable.fbx"};
    ECS::AddComponent<Renderer>(tableHandle, tableMesh, tableMaterial);

    // Pieces
    prefab::Create<prefab::Boar>({0.2f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Boar Piece");
    prefab::Create<prefab::Cattle>({0.4f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Cattle Piece");
    prefab::Create<prefab::Coal>({0.2f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Coal Piece");    
    prefab::Create<prefab::CoinOne>({0.8f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "OneCoin Piece");
    prefab::Create<prefab::CoinTen>({1.0f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "TenCoin Piece");
    prefab::Create<prefab::CoinTwo>({0.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "TwoCoin Piece");
    prefab::Create<prefab::Dog>(Vector3::Zero(), {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Dog Piece");
    prefab::Create<prefab::Donkey>({0.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Donkey Piece");
    //prefab::Create<prefab::Dwarf>
    prefab::Create<prefab::Grain>({0.6f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Grain Piece");
    prefab::Create<prefab::Ruby>({0.4f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Ruby Piece");
    prefab::Create<prefab::Sheep>({1.0f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Sheep Piece");
    //prefab::Create<prefab::Stable>
    prefab::Create<prefab::StartingPlayerPiece>({1.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Starting Player Piece");
    prefab::Create<prefab::Stone>({0.6f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stone Piece");
    prefab::Create<prefab::Vegetable>({0.8f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Vegetable Piece");
    prefab::Create<prefab::Wood>({0.8f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Wood Piece");

    // Stable Piece 1
    auto stableMaterial1 = graphics::PBRMaterial{{"project//Textures//PlayerRed.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle1 = ECS::CreateEntity({0.2f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    auto stableMesh = graphics::Mesh{"project//Models//StablePiece.fbx"};
    ECS::AddComponent<Renderer>(stableHandle1, stableMesh, stableMaterial1);

    // Stable Piece 2
    auto stableMaterial2 = graphics::PBRMaterial{{"project//Textures//PlayerOrange.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle2 = ECS::CreateEntity({0.4f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle2, stableMesh, stableMaterial2);

    // Stable Piece 3
    auto stableMaterial3 = graphics::PBRMaterial{{"project//Textures//PlayerYellow.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle3 = ECS::CreateEntity({0.6f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle3, stableMesh, stableMaterial3);

    // Stable Piece 4
    auto stableMaterial4 = graphics::PBRMaterial{{"project//Textures//PlayerGreen.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle4 = ECS::CreateEntity({0.8f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle4, stableMesh, stableMaterial4);

    // Stable Piece 5
    auto stableMaterial5 = graphics::PBRMaterial{{"project//Textures//PlayerTeal.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle5 = ECS::CreateEntity({1.0f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle5, stableMesh, stableMaterial5);

    // Stable Piece 6
    auto stableMaterial6 = graphics::PBRMaterial{{"project//Textures//PlayerBlue.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle6 = ECS::CreateEntity({0.2f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle6, stableMesh, stableMaterial6);

    // Stable Piece 7
    auto stableMaterial7 = graphics::PBRMaterial{{"project//Textures//PlayerPurple.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle7 = ECS::CreateEntity({0.4f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    ECS::AddComponent<Renderer>(stableHandle7, stableMesh, stableMaterial7);

    // Dwarf Disc Piece 1
    auto dwarfDiscMaterial1 = graphics::PBRMaterial{{"project//Textures//PlayerRed.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle1 = ECS::CreateEntity({0.6f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 1");
    auto dwarfDiscMesh = graphics::Mesh{"project//Models//Disc.fbx"};
    ECS::AddComponent<Renderer>(dwarfDiscHandle1, dwarfDiscMesh, dwarfDiscMaterial1);

    // DwarfDisc Piece 2
    auto dwarfDiscMaterial2 = graphics::PBRMaterial{{"project//Textures//PlayerOrange.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle2 = ECS::CreateEntity({0.8f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 2");
    ECS::AddComponent<Renderer>(dwarfDiscHandle2, dwarfDiscMesh, dwarfDiscMaterial2);

    // DwarfDisc Piece 3
    auto dwarfDiscMaterial3 = graphics::PBRMaterial{{"project//Textures//PlayerYellow.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle3 = ECS::CreateEntity({1.0f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 3");
    ECS::AddComponent<Renderer>(dwarfDiscHandle3, dwarfDiscMesh, dwarfDiscMaterial3);

    // DwarfDisc Piece 4
    auto dwarfDiscMaterial4 = graphics::PBRMaterial{{"project//Textures//PlayerGreen.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle4 = ECS::CreateEntity({0.0f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 4");
    ECS::AddComponent<Renderer>(dwarfDiscHandle4, dwarfDiscMesh, dwarfDiscMaterial4);

    // DwarfDisc Piece 5
    auto dwarfDiscMaterial5 = graphics::PBRMaterial{{"project//Textures//PlayerTeal.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle5 = ECS::CreateEntity({0.2f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 5");
    ECS::AddComponent<Renderer>(dwarfDiscHandle5, dwarfDiscMesh, dwarfDiscMaterial5);

    // DwarfDisc Piece 6
    auto dwarfDiscMaterial6 = graphics::PBRMaterial{{"project//Textures//PlayerBlue.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle6 = ECS::CreateEntity({0.4f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 6");
    ECS::AddComponent<Renderer>(dwarfDiscHandle6, dwarfDiscMesh, dwarfDiscMaterial6);

    // DwarfDisc Piece 7
    auto dwarfDiscMaterial7 = graphics::PBRMaterial{{"project//Textures//PlayerPurple.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle7 = ECS::CreateEntity({0.6f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 7");
    ECS::AddComponent<Renderer>(dwarfDiscHandle7, dwarfDiscMesh, dwarfDiscMaterial7);


    // Furnishing Tiles
    prefab::Create<prefab::FurnishingTileAdditionalDwelling>({0.4f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Additional Dwelling Piece");
    prefab::Create<prefab::FurnishingTileCoupleDwelling>({0.8f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Couple Dwelling Piece");
    prefab::Create<prefab::FurnishingTileDwelling>({1.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Dwelling Piece");
    prefab::Create<prefab::FurnishingTileBeerParlor>({1.6f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BeerParlor Piece");
    prefab::Create<prefab::FurnishingTileBlacksmithingParlor>({2.0f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BlacksmithingParlor Piece");
    prefab::Create<prefab::FurnishingTileBlacksmith>({2.4f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Blacksmith Piece");
    prefab::Create<prefab::FurnishingTileBreakfastRoom>({2.8f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BreakfastRoom Piece");
    prefab::Create<prefab::FurnishingTileBreedingCave>({3.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BreedingCave Piece");
    prefab::Create<prefab::FurnishingTileBroomChamber>({3.6f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BroomChamber Piece");
    prefab::Create<prefab::FurnishingTileBuilder>({4.0f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Builder Piece");
    prefab::Create<prefab::FurnishingTileCarpenter>({0.4f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Carpenter Piece");
    prefab::Create<prefab::FurnishingTileCookingCave>({0.8f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "CookingCave Piece");
    prefab::Create<prefab::FurnishingTileCuddleRoom>({1.2f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "CuddleRoom Piece");
    prefab::Create<prefab::FurnishingTileDogSchool>({1.6f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "DogSchool Piece");
    prefab::Create<prefab::FurnishingTileFodderChamber>({2.0f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FodderChamber Piece");
    prefab::Create<prefab::FurnishingTileFoodChamber>({2.4f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FoodChamber Piece");
    prefab::Create<prefab::FurnishingTileGuestRoom>({2.8f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "GuestRoom Piece");
    prefab::Create<prefab::FurnishingTileHuntingParlor>({3.2f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "HuntingParlor Piece");
    prefab::Create<prefab::FurnishingTileMainStorage>({3.6f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MainStorage Piece");
    prefab::Create<prefab::FurnishingTileMilkingParlor>({4.0f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MilkingParlor Piece");
    prefab::Create<prefab::FurnishingTileMiner>({0.4f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Miner Piece");
    prefab::Create<prefab::FurnishingTileMiningCave>({0.8f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MiningCave Piece");
    prefab::Create<prefab::FurnishingTileMixedDwelling>({1.2f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MixedDwelling Piece");
    prefab::Create<prefab::FurnishingTileOfficeRoom>({1.6f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "OfficeRoom Piece");
    prefab::Create<prefab::FurnishingTileOreStorage>({2.0f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "OreStorage Piece");
    prefab::Create<prefab::FurnishingTilePeacefulCave>({2.4f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "PeacefulCave Piece");
    prefab::Create<prefab::FurnishingTilePrayerChamber>({2.8f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "PrayerChamber Piece");
    prefab::Create<prefab::FurnishingTileQuarry>({3.2f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Quarry Piece");
    prefab::Create<prefab::FurnishingTileRubySupplier>({3.6f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "RubySupplier Piece");
    prefab::Create<prefab::FurnishingTileSeam>({4.0f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Seam Piece");
    prefab::Create<prefab::FurnishingTileSimpleDwelling>({0.8f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SimpleDwelling Piece");
    prefab::Create<prefab::FurnishingTileSimpleDwelling2>({0.4f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SimpleDwelling2 Piece");
    prefab::Create<prefab::FurnishingTileSlaughterCave>({1.2f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SlaughteringCave Piece");
    prefab::Create<prefab::FurnishingTileSparePartStorage>({1.6f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SparePartStorage Piece");
    prefab::Create<prefab::FurnishingTileStateParlor>({2.0f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StateParlor Piece");
    prefab::Create<prefab::FurnishingTileStoneCarver>({2.4f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneCarver Piece");
    prefab::Create<prefab::FurnishingTileStoneStorage>({2.8f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneStorage Piece");
    prefab::Create<prefab::FurnishingTileStoneSupplier>({3.2f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneSupplier Piece");
    prefab::Create<prefab::FurnishingTileStubbleRoom>({3.6f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StubbleRoom Piece");
    prefab::Create<prefab::FurnishingTileSupplierStorage>({4.0f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SuppliesStorage Piece");
    prefab::Create<prefab::FurnishingTileTrader>({0.4f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Trader Piece");
    prefab::Create<prefab::FurnishingTileTreasureChamber>({0.8f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "TreasureChamber Piece");
    prefab::Create<prefab::FurnishingTileWeaponStorage>({1.2f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WeaponStorage Piece");
    prefab::Create<prefab::FurnishingTileWeavingParlor>({1.6f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WeavingParlor Piece");
    prefab::Create<prefab::FurnishingTileWoodSupplier>({2.0f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WoodSupplier Piece");
    prefab::Create<prefab::FurnishingTileWorkingCave>({2.4f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WorkingCave Piece");
    prefab::Create<prefab::FurnishingTileWorkRoom>({2.8f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WorkRoom Piece");
    prefab::Create<prefab::FurnishingTileWritingChamber>({3.2f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WritingChamber Piece");
}
 
void InitialScene::Unload()
{
    
}