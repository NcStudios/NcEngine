#include "InitialScene.h"
#include "NcCamera.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "CamController.h"
#include "DebugUtils.h"
#include "GamePiece.h"
#include "ClickHandler.h"

using namespace nc;
using namespace project;

void InitialScene::Load()
{
    // Light
    auto lvHandle = NcCreateEntity({-33.9f, 10.3f, -2.4f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    NcAddEngineComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = NcCreateEntity({0.0f, 5.0f, 0.0f}, {1.3f, 0.0f, 0.0f}, Vector3::Zero(), "Main Camera");
    auto camComponentPtr = NcAddUserComponent<Camera>(camHandle);
    NcRegisterMainCamera(camComponentPtr);
    NcAddUserComponent<CamController>(camHandle);
    NcAddUserComponent<ClickHandler>(camHandle);
    
    // Debug Controller
    auto debugHandle = NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
    NcAddUserComponent<SceneReset>(debugHandle);
    NcAddUserComponent<Timer>(debugHandle);

    const auto scaleFactor = 2;

    // Table
    auto tableMaterial = graphics::PBRMaterial{{"project//Textures//DiningRoomTable_Material_BaseColor.png", "project//Textures//DiningRoomTable_Material_Normal.png",  "project//Textures//DiningRoomTable_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto tableHandle = NcCreateEntity({2.0f  * scaleFactor, -0.4f, 1.5f * scaleFactor}, {1.5708f, 0.0f, 1.5708f}, Vector3::One() * 7.5, "Table Piece");
    auto tableMesh = graphics::Mesh{"project//Models//DiningRoomTable.fbx"};
    NcAddEngineComponent<Renderer>(tableHandle, tableMesh, tableMaterial);

    // Coal Piece
    auto coalMaterial = graphics::PBRMaterial{{"project//Textures//CoalPiece_Material_BaseColor.png", "project//Textures//CoalPiece_Material_Normal.png",  "project//Textures//CoalPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto coalHandle = NcCreateEntity({0.2f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Coal Piece");
    auto coalMesh = graphics::Mesh{"project//Models//CoalPiece.fbx"};
    NcAddEngineComponent<Renderer>(coalHandle, coalMesh, coalMaterial);

    // Ruby Piece
    auto rubyMaterial = graphics::PBRMaterial{{"project//Textures//RubyPiece_Material_BaseColor.png", "project//Textures//RubyPiece_Material_Normal.png",  "project//Textures//RubyPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto rubyHandle = NcCreateEntity({0.4f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Ruby Piece");
    auto rubyMesh = graphics::Mesh{"project//Models//RubyPiece.fbx"};
    NcAddEngineComponent<Renderer>(rubyHandle, rubyMesh, rubyMaterial);

    // Stone Piece
    auto stoneMaterial = graphics::PBRMaterial{{"project//Textures//StonePiece_Material_BaseColor.png", "project//Textures//StonePiece_Material_Normal.png",  "project//Textures//StonePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stoneHandle = NcCreateEntity({0.6f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stone Piece");
    auto stoneMesh = graphics::Mesh{"project//Models//StonePiece.fbx"};
    NcAddEngineComponent<Renderer>(stoneHandle, stoneMesh, stoneMaterial);
    auto stoneEntity = NcGetEntity(stoneHandle);
    auto stoneTransform = NcGetTransform(stoneEntity->Handles.transform);
    NcAddUserComponent<GamePiece>(stoneHandle, stoneTransform);

    // Wood Piece
    auto woodMaterial = graphics::PBRMaterial{{"project//Textures//WoodPiece_Material_BaseColor.png", "project//Textures//WoodPiece_Material_Normal.png",  "project//Textures//WoodPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto woodHandle = NcCreateEntity({0.8f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Wood Piece");
    auto woodMesh = graphics::Mesh{"project//Models//WoodPiece.fbx"};
    NcAddEngineComponent<Renderer>(woodHandle, woodMesh, woodMaterial);
    auto woodEntity = NcGetEntity(woodHandle);
    auto woodTransform = NcGetTransform(woodEntity->Handles.transform);
    NcAddUserComponent<GamePiece>(woodHandle, woodTransform);

    // Dog Piece
    auto dogMaterial = graphics::PBRMaterial{{"project//Textures//DogPiece_Material_BaseColor.png", "project//Textures//DogPiece_Material_Normal.png",  "project//Textures//DogPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dogHandle = NcCreateEntity(Vector3::Zero(), {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Dog Piece");
    auto dogMesh = graphics::Mesh{"project//Models//DogPiece.fbx"};
    NcAddEngineComponent<Renderer>(dogHandle, dogMesh, dogMaterial);

    // Sheep Piece
    auto sheepMaterial = graphics::PBRMaterial{{"project//Textures//SheepPiece_Material_BaseColor.png", "project//Textures//SheepPiece_Material_Normal.png",  "project//Textures//SheepPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto sheepHandle = NcCreateEntity({1.0f * scaleFactor, 0.0f, 0.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Sheep Piece");
    auto sheepMesh = graphics::Mesh{"project//Models//SheepPiece.fbx"};
    NcAddEngineComponent<Renderer>(sheepHandle, sheepMesh, sheepMaterial);

    // Donkey Piece
    auto donkeyMaterial = graphics::PBRMaterial{{"project//Textures//DonkeyPiece_Material_BaseColor.png", "project//Textures//DonkeyPiece_Material_Normal.png",  "project//Textures//DonkeyPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto donkeyHandle = NcCreateEntity({0.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Donkey Piece");
    auto donkeyMesh = graphics::Mesh{"project//Models//DonkeyPiece.fbx"};
    NcAddEngineComponent<Renderer>(donkeyHandle, donkeyMesh, donkeyMaterial);

    // Boar Piece
    auto boarMaterial = graphics::PBRMaterial{{"project//Textures//BoarPiece_Material_BaseColor.png", "project//Textures//BoarPiece_Material_Normal.png",  "project//Textures//BoarPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto boarHandle = NcCreateEntity({0.2f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Boar Piece");
    auto boarMesh = graphics::Mesh{"project//Models//BoarPiece.fbx"};
    NcAddEngineComponent<Renderer>(boarHandle, boarMesh, boarMaterial);

    // Cattle Piece
    auto cattleMaterial = graphics::PBRMaterial{{"project//Textures//CattlePiece_Material_BaseColor.png", "project//Textures//CattlePiece_Material_Normal.png",  "project//Textures//CattlePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto cattleHandle = NcCreateEntity({0.4f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Cattle Piece");
    auto cattleMesh = graphics::Mesh{"project//Models//CattlePiece.fbx"};
    NcAddEngineComponent<Renderer>(cattleHandle, cattleMesh, cattleMaterial);

    // Grain Piece
    auto grainMaterial = graphics::PBRMaterial{{"project//Textures//GrainPiece_Material_BaseColor.png", "project//Textures//GrainPiece_Material_Normal.png",  "project//Textures//GrainPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto grainHandle = NcCreateEntity({0.6f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Grain Piece");
    auto grainMesh = graphics::Mesh{"project//Models//GrainPiece.fbx"};
    NcAddEngineComponent<Renderer>(grainHandle, grainMesh, grainMaterial);

    // Vegetable Piece
    auto vegetableMaterial = graphics::PBRMaterial{{"project//Textures//VegetablePiece_Material_BaseColor.png", "project//Textures//VegetablePiece_Material_Normal.png",  "project//Textures//VegetablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto vegetableHandle = NcCreateEntity({0.8f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Vegetable Piece");
    auto vegetableMesh = graphics::Mesh{"project//Models//VegetablePiece.fbx"};
    NcAddEngineComponent<Renderer>(vegetableHandle, vegetableMesh, vegetableMaterial);

    // Starting Player Piece
    auto startingPlayerMaterial = graphics::PBRMaterial{{"project//Textures//StartingPlayerPiece_Material_BaseColor.png", "project//Textures//StartingPlayerPiece_Material_Normal.png",  "project//Textures//StartingPlayerPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto startingPlayerHandle = NcCreateEntity({1.0f * scaleFactor, 0.0f, 0.2f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Starting Player Piece");
    auto startingPlayerMesh = graphics::Mesh{"project//Models//StartingPlayerPiece.fbx"};
    NcAddEngineComponent<Renderer>(startingPlayerHandle, startingPlayerMesh, startingPlayerMaterial);

    // Stable Piece 1
    auto stableMaterial1 = graphics::PBRMaterial{{"project//Textures//PlayerRed.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle1 = NcCreateEntity({0.2f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    auto stableMesh = graphics::Mesh{"project//Models//StablePiece.fbx"};
    NcAddEngineComponent<Renderer>(stableHandle1, stableMesh, stableMaterial1);

    // Stable Piece 2
    auto stableMaterial2 = graphics::PBRMaterial{{"project//Textures//PlayerOrange.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle2 = NcCreateEntity({0.4f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle2, stableMesh, stableMaterial2);

    // Stable Piece 3
    auto stableMaterial3 = graphics::PBRMaterial{{"project//Textures//PlayerYellow.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle3 = NcCreateEntity({0.6f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle3, stableMesh, stableMaterial3);

    // Stable Piece 4
    auto stableMaterial4 = graphics::PBRMaterial{{"project//Textures//PlayerGreen.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle4 = NcCreateEntity({0.8f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle4, stableMesh, stableMaterial4);

    // Stable Piece 5
    auto stableMaterial5 = graphics::PBRMaterial{{"project//Textures//PlayerTeal.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle5 = NcCreateEntity({1.0f * scaleFactor, 0.0f, 0.4f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle5, stableMesh, stableMaterial5);

    // Stable Piece 6
    auto stableMaterial6 = graphics::PBRMaterial{{"project//Textures//PlayerBlue.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle6 = NcCreateEntity({0.2f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle6, stableMesh, stableMaterial6);

    // Stable Piece 7
    auto stableMaterial7 = graphics::PBRMaterial{{"project//Textures//PlayerPurple.png", "project//Textures//StablePiece_Material_Normal.png",  "project//Textures//StablePiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto stableHandle7 = NcCreateEntity({0.4f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "Stable Piece");
    NcAddEngineComponent<Renderer>(stableHandle7, stableMesh, stableMaterial7);

    // Dwarf Disc Piece 1
    auto dwarfDiscMaterial1 = graphics::PBRMaterial{{"project//Textures//PlayerRed.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle1 = NcCreateEntity({0.6f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 1");
    auto dwarfDiscMesh = graphics::Mesh{"project//Models//DwarfDisc.fbx"};
    NcAddEngineComponent<Renderer>(dwarfDiscHandle1, dwarfDiscMesh, dwarfDiscMaterial1);

    // DwarfDisc Piece 2
    auto dwarfDiscMaterial2 = graphics::PBRMaterial{{"project//Textures//PlayerOrange.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle2 = NcCreateEntity({0.8f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 2");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle2, dwarfDiscMesh, dwarfDiscMaterial2);

    // DwarfDisc Piece 3
    auto dwarfDiscMaterial3 = graphics::PBRMaterial{{"project//Textures//PlayerYellow.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle3 = NcCreateEntity({1.0f * scaleFactor, 0.0f, 0.6f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 3");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle3, dwarfDiscMesh, dwarfDiscMaterial3);

    // DwarfDisc Piece 4
    auto dwarfDiscMaterial4 = graphics::PBRMaterial{{"project//Textures//PlayerGreen.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle4 = NcCreateEntity({0.0f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 4");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle4, dwarfDiscMesh, dwarfDiscMaterial4);

    // DwarfDisc Piece 5
    auto dwarfDiscMaterial5 = graphics::PBRMaterial{{"project//Textures//PlayerTeal.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle5 = NcCreateEntity({0.2f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 5");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle5, dwarfDiscMesh, dwarfDiscMaterial5);

    // DwarfDisc Piece 6
    auto dwarfDiscMaterial6 = graphics::PBRMaterial{{"project//Textures//PlayerBlue.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle6 = NcCreateEntity({0.4f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 6");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle6, dwarfDiscMesh, dwarfDiscMaterial6);

    // DwarfDisc Piece 7
    auto dwarfDiscMaterial7 = graphics::PBRMaterial{{"project//Textures//PlayerPurple.png", "project//Textures//DwarfDisc_Material_Normal.png",  "project//Textures//DwarfDisc_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto dwarfDiscHandle7 = NcCreateEntity({0.6f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "DwarfDisc Piece 7");
    NcAddEngineComponent<Renderer>(dwarfDiscHandle7, dwarfDiscMesh, dwarfDiscMaterial7);

    // One Coin Piece
    auto oneCoinPieceMaterial = graphics::PBRMaterial{{"project//Textures//OneCoinPiece_Material_BaseColor.png", "project//Textures//OneCoinPiece_Material_Normal.png",  "project//Textures//OneCoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto oneCoinPieceHandle = NcCreateEntity({0.8f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "OneCoin Piece");
    NcAddEngineComponent<Renderer>(oneCoinPieceHandle, dwarfDiscMesh, oneCoinPieceMaterial);

    // Ten Coin Piece
    auto tenCoinPieceMaterial = graphics::PBRMaterial{{"project//Textures//TenCoinPiece_Material_BaseColor.png", "project//Textures//OneCoinPiece_Material_Normal.png",  "project//Textures//OneCoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto tenCoinPieceHandle = NcCreateEntity({1.0f * scaleFactor, 0.0f, 0.8f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "TenCoin Piece");
    NcAddEngineComponent<Renderer>(tenCoinPieceHandle, dwarfDiscMesh, tenCoinPieceMaterial);

    // Two Coin Piece
    auto twoCoinPieceMaterial = graphics::PBRMaterial{{"project//Textures//TwoCoinPiece_Material_BaseColor.png", "project//Textures//OneCoinPiece_Material_Normal.png",  "project//Textures//OneCoinPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto twoCoinPieceHandle = NcCreateEntity({0.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 0.0f, 0.0f}, Vector3::One() * scaleFactor, "TwoCoin Piece");
    NcAddEngineComponent<Renderer>(twoCoinPieceHandle, dwarfDiscMesh, twoCoinPieceMaterial);

    // Furnishing Tile - Additional Dwelling
    auto ftAdditionalDwellingMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png",  "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMesh = graphics::Mesh{"project//Models//FurnishingTile.fbx"};
    auto ftAdditionalDwellingHandle = NcCreateEntity({0.4f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Additional Dwelling Piece");
    NcAddEngineComponent<Renderer>(ftAdditionalDwellingHandle, ftMesh, ftAdditionalDwellingMaterial);

    // Furnishing Tile - Couple Dwelling
    auto ftCoupleDwellingMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_CoupleDwelling_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png",  "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftCoupleDwellingHandle = NcCreateEntity({0.8f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Couple Dwelling Piece");
    NcAddEngineComponent<Renderer>(ftCoupleDwellingHandle, ftMesh, ftCoupleDwellingMaterial);

    // Furnishing Tile - Dwelling
    auto ftDwellingMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Dwelling_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png",  "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftDwellingHandle = NcCreateEntity({1.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FT Dwelling Piece");
    NcAddEngineComponent<Renderer>(ftDwellingHandle, ftMesh, ftDwellingMaterial);

    // Furnishing Tile - BeerParlor
    auto ftBeerParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_BeerParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBeerParlorHandle =  NcCreateEntity({1.6f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BeerParlor Piece");
    NcAddEngineComponent<Renderer>(ftBeerParlorHandle, ftMesh, ftBeerParlorMaterial);

    // Furnishing Tile - BlacksmithingParlor
    auto ftBlacksmithingParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_BlacksmithingParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBlacksmithingParlorHandle =  NcCreateEntity({2.0f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BlacksmithingParlor Piece");
    NcAddEngineComponent<Renderer>(ftBlacksmithingParlorHandle, ftMesh, ftBlacksmithingParlorMaterial);

    // Furnishing Tile - Blacksmith
    auto ftBlacksmithMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Blacksmith_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBlacksmithHandle =  NcCreateEntity({2.4f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Blacksmith Piece");
    NcAddEngineComponent<Renderer>(ftBlacksmithHandle, ftMesh, ftBlacksmithMaterial);

    // Furnishing Tile - BreakfastRoom
    auto ftBreakfastRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_BreakfastRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBreakfastRoomHandle =  NcCreateEntity({2.8f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BreakfastRoom Piece");
    NcAddEngineComponent<Renderer>(ftBreakfastRoomHandle, ftMesh, ftBreakfastRoomMaterial);

    // Furnishing Tile - BreedingCave
    auto ftBreedingCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_BreedingCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBreedingCaveHandle =  NcCreateEntity({3.2f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BreedingCave Piece");
    NcAddEngineComponent<Renderer>(ftBreedingCaveHandle, ftMesh, ftBreedingCaveMaterial);

    // Furnishing Tile - BroomChamber
    auto ftBroomChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_BroomChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBroomChamberHandle =  NcCreateEntity({3.6f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "BroomChamber Piece");
    NcAddEngineComponent<Renderer>(ftBroomChamberHandle, ftMesh, ftBroomChamberMaterial);

    // Furnishing Tile - Builder
    auto ftBuilderMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Builder_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftBuilderHandle =  NcCreateEntity({4.0f * scaleFactor, 0.0f, 1.0f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Builder Piece");
    NcAddEngineComponent<Renderer>(ftBuilderHandle, ftMesh, ftBuilderMaterial);

    // Furnishing Tile - Carpenter
    auto ftCarpenterMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Carpenter_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftCarpenterHandle =  NcCreateEntity({0.4f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Carpenter Piece");
    NcAddEngineComponent<Renderer>(ftCarpenterHandle, ftMesh, ftCarpenterMaterial);

    // Furnishing Tile - CookingCave
    auto ftCookingCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_CookingCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftCookingCaveHandle =  NcCreateEntity({0.8f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "CookingCave Piece");
    NcAddEngineComponent<Renderer>(ftCookingCaveHandle, ftMesh, ftCookingCaveMaterial);

    // Furnishing Tile - CuddleRoom
    auto ftCuddleRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_CuddleRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftCuddleRoomHandle =  NcCreateEntity({1.2f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "CuddleRoom Piece");
    NcAddEngineComponent<Renderer>(ftCuddleRoomHandle, ftMesh, ftCuddleRoomMaterial);

    // Furnishing Tile - DogSchool
    auto ftDogSchoolMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_DogSchool_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftDogSchoolHandle =  NcCreateEntity({1.6f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "DogSchool Piece");
    NcAddEngineComponent<Renderer>(ftDogSchoolHandle, ftMesh, ftDogSchoolMaterial);

    // Furnishing Tile - FodderChamber
    auto ftFodderChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_FodderChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftFodderChamberHandle =  NcCreateEntity({2.0f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FodderChamber Piece");
    NcAddEngineComponent<Renderer>(ftFodderChamberHandle, ftMesh, ftFodderChamberMaterial);

    // Furnishing Tile - FoodChamber
    auto ftFoodChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_FoodChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftFoodChamberHandle =  NcCreateEntity({2.4f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "FoodChamber Piece");
    NcAddEngineComponent<Renderer>(ftFoodChamberHandle, ftMesh, ftFoodChamberMaterial);

    // Furnishing Tile - GuestRoom
    auto ftGuestRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_GuestRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftGuestRoomHandle =  NcCreateEntity({2.8f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "GuestRoom Piece");
    NcAddEngineComponent<Renderer>(ftGuestRoomHandle, ftMesh, ftGuestRoomMaterial);

    // Furnishing Tile - HuntingParlor
    auto ftHuntingParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_HuntingParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftHuntingParlorHandle =  NcCreateEntity({3.2f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "HuntingParlor Piece");
    NcAddEngineComponent<Renderer>(ftHuntingParlorHandle, ftMesh, ftHuntingParlorMaterial);

    // Furnishing Tile - MainStorage
    auto ftMainStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_MainStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMainStorageHandle =  NcCreateEntity({3.6f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MainStorage Piece");
    NcAddEngineComponent<Renderer>(ftMainStorageHandle, ftMesh, ftMainStorageMaterial);

    // Furnishing Tile - MilkingParlor
    auto ftMilkingParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_MilkingParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMilkingParlorHandle =  NcCreateEntity({4.0f * scaleFactor, 0.0f, 1.4f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MilkingParlor Piece");
    NcAddEngineComponent<Renderer>(ftMilkingParlorHandle, ftMesh, ftMilkingParlorMaterial);

    // Furnishing Tile - Miner
    auto ftMinerMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Miner_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMinerHandle =  NcCreateEntity({0.4f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Miner Piece");
    NcAddEngineComponent<Renderer>(ftMinerHandle, ftMesh, ftMinerMaterial);

    // Furnishing Tile - MiningCave
    auto ftMiningCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_MiningCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMiningCaveHandle =  NcCreateEntity({0.8f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MiningCave Piece");
    NcAddEngineComponent<Renderer>(ftMiningCaveHandle, ftMesh, ftMiningCaveMaterial);

    // Furnishing Tile - MixedDwelling
    auto ftMixedDwellingMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_MixedDwelling_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftMixedDwellingHandle =  NcCreateEntity({1.2f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "MixedDwelling Piece");
    NcAddEngineComponent<Renderer>(ftMixedDwellingHandle, ftMesh, ftMixedDwellingMaterial);

    // Furnishing Tile - OfficeRoom
    auto ftOfficeRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_OfficeRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftOfficeRoomHandle =  NcCreateEntity({1.6f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "OfficeRoom Piece");
    NcAddEngineComponent<Renderer>(ftOfficeRoomHandle, ftMesh, ftOfficeRoomMaterial);

    // Furnishing Tile - OreStorage
    auto ftOreStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_OreStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftOreStorageHandle =  NcCreateEntity({2.0f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "OreStorage Piece");
    NcAddEngineComponent<Renderer>(ftOreStorageHandle, ftMesh, ftOreStorageMaterial);

    // Furnishing Tile - PeacefulCave
    auto ftPeacefulCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_PeacefulCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftPeacefulCaveHandle =  NcCreateEntity({2.4f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "PeacefulCave Piece");
    NcAddEngineComponent<Renderer>(ftPeacefulCaveHandle, ftMesh, ftPeacefulCaveMaterial);

    // Furnishing Tile - PrayerChamber
    auto ftPrayerChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_PrayerChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftPrayerChamberHandle =  NcCreateEntity({2.8f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "PrayerChamber Piece");
    NcAddEngineComponent<Renderer>(ftPrayerChamberHandle, ftMesh, ftPrayerChamberMaterial);

    // Furnishing Tile - Quarry
    auto ftQuarryMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Quarry_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftQuarryHandle =  NcCreateEntity({3.2f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Quarry Piece");
    NcAddEngineComponent<Renderer>(ftQuarryHandle, ftMesh, ftQuarryMaterial);

    // Furnishing Tile - RubySupplier
    auto ftRubySupplierMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_RubySupplier_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftRubySupplierHandle =  NcCreateEntity({3.6f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "RubySupplier Piece");
    NcAddEngineComponent<Renderer>(ftRubySupplierHandle, ftMesh, ftRubySupplierMaterial);

    // Furnishing Tile - Seam
    auto ftSeamMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Seam_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSeamHandle =  NcCreateEntity({4.0f * scaleFactor, 0.0f, 1.8f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Seam Piece");
    NcAddEngineComponent<Renderer>(ftSeamHandle, ftMesh, ftSeamMaterial);

    // Furnishing Tile - SimpleDwelling2
    auto ftSimpleDwelling2Material = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_SimpleDwelling2_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSimpleDwelling2Handle =  NcCreateEntity({0.4f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SimpleDwelling2 Piece");
    NcAddEngineComponent<Renderer>(ftSimpleDwelling2Handle, ftMesh, ftSimpleDwelling2Material);

    // Furnishing Tile - SimpleDwelling
    auto ftSimpleDwellingMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_SimpleDwelling_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSimpleDwellingHandle =  NcCreateEntity({0.8f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SimpleDwelling Piece");
    NcAddEngineComponent<Renderer>(ftSimpleDwellingHandle, ftMesh, ftSimpleDwellingMaterial);

    // Furnishing Tile - SlaughteringCave
    auto ftSlaughteringCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_SlaughteringCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSlaughteringCaveHandle =  NcCreateEntity({1.2f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SlaughteringCave Piece");
    NcAddEngineComponent<Renderer>(ftSlaughteringCaveHandle, ftMesh, ftSlaughteringCaveMaterial);

    // Furnishing Tile - SparePartStorage
    auto ftSparePartStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_SparePartStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSparePartStorageHandle =  NcCreateEntity({1.6f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SparePartStorage Piece");
    NcAddEngineComponent<Renderer>(ftSparePartStorageHandle, ftMesh, ftSparePartStorageMaterial);

    // Furnishing Tile - StateParlor
    auto ftStateParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_StateParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftStateParlorHandle =  NcCreateEntity({2.0f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StateParlor Piece");
    NcAddEngineComponent<Renderer>(ftStateParlorHandle, ftMesh, ftStateParlorMaterial);

    // Furnishing Tile - StoneCarver
    auto ftStoneCarverMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_StoneCarver_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftStoneCarverHandle =  NcCreateEntity({2.4f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneCarver Piece");
    NcAddEngineComponent<Renderer>(ftStoneCarverHandle, ftMesh, ftStoneCarverMaterial);

    // Furnishing Tile - StoneStorage
    auto ftStoneStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_StoneStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftStoneStorageHandle =  NcCreateEntity({2.8f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneStorage Piece");
    NcAddEngineComponent<Renderer>(ftStoneStorageHandle, ftMesh, ftStoneStorageMaterial);

    // Furnishing Tile - StoneSupplier
    auto ftStoneSupplierMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_StoneSupplier_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftStoneSupplierHandle =  NcCreateEntity({3.2f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StoneSupplier Piece");
    NcAddEngineComponent<Renderer>(ftStoneSupplierHandle, ftMesh, ftStoneSupplierMaterial);

    // Furnishing Tile - StubbleRoom
    auto ftStubbleRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_StubbleRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftStubbleRoomHandle =  NcCreateEntity({3.6f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "StubbleRoom Piece");
    NcAddEngineComponent<Renderer>(ftStubbleRoomHandle, ftMesh, ftStubbleRoomMaterial);

    // Furnishing Tile - SuppliesStorage
    auto ftSuppliesStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_SuppliesStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftSuppliesStorageHandle =  NcCreateEntity({4.0f * scaleFactor, 0.0f, 2.2f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "SuppliesStorage Piece");
    NcAddEngineComponent<Renderer>(ftSuppliesStorageHandle, ftMesh, ftSuppliesStorageMaterial);

    // Furnishing Tile - Trader
    auto ftTraderMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_Trader_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftTraderHandle =  NcCreateEntity({0.4f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "Trader Piece");
    NcAddEngineComponent<Renderer>(ftTraderHandle, ftMesh, ftTraderMaterial);

    // Furnishing Tile - TreasureChamber
    auto ftTreasureChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_TreasureChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftTreasureChamberHandle =  NcCreateEntity({0.8f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "TreasureChamber Piece");
    NcAddEngineComponent<Renderer>(ftTreasureChamberHandle, ftMesh, ftTreasureChamberMaterial);

    // Furnishing Tile - WeaponStorage
    auto ftWeaponStorageMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WeaponStorage_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWeaponStorageHandle =  NcCreateEntity({1.2f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WeaponStorage Piece");
    NcAddEngineComponent<Renderer>(ftWeaponStorageHandle, ftMesh, ftWeaponStorageMaterial);

    // Furnishing Tile - WeavingParlor
    auto ftWeavingParlorMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WeavingParlor_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWeavingParlorHandle =  NcCreateEntity({1.6f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WeavingParlor Piece");
    NcAddEngineComponent<Renderer>(ftWeavingParlorHandle, ftMesh, ftWeavingParlorMaterial);

    // Furnishing Tile - WoodSupplier
    auto ftWoodSupplierMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WoodSupplier_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWoodSupplierHandle =  NcCreateEntity({2.0f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WoodSupplier Piece");
    NcAddEngineComponent<Renderer>(ftWoodSupplierHandle, ftMesh, ftWoodSupplierMaterial);

    // Furnishing Tile - WorkingCave
    auto ftWorkingCaveMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WorkingCave_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWorkingCaveHandle =  NcCreateEntity({2.4f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WorkingCave Piece");
    NcAddEngineComponent<Renderer>(ftWorkingCaveHandle, ftMesh, ftWorkingCaveMaterial);

    // Furnishing Tile - WorkRoom
    auto ftWorkRoomMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WorkRoom_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWorkRoomHandle =  NcCreateEntity({2.8f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WorkRoom Piece");
    NcAddEngineComponent<Renderer>(ftWorkRoomHandle, ftMesh, ftWorkRoomMaterial);

    // Furnishing Tile - WritingChamber
    auto ftWritingChamberMaterial = graphics::PBRMaterial{{"project//Textures//FurnishingTiles//FT_WritingChamber_Material_BaseColor.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Normal.png", "project//Textures//FurnishingTiles//FT_AdditionalDwelling_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ftWritingChamberHandle =  NcCreateEntity({3.2f * scaleFactor, 0.0f, 2.6f * scaleFactor}, {1.5708f, 1.5708f, 0.0f}, Vector3::One() * scaleFactor * 2, "WritingChamber Piece");
    NcAddEngineComponent<Renderer>(ftWritingChamberHandle, ftMesh, ftWritingChamberMaterial);
}
 
void InitialScene::Unload()
{
    
}