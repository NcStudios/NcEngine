#include "InitialScene.h"
#include "component/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "component/PointLight.h"
#include "NcCamera.h"
#include "CamController.h"
#include "DebugUtils.h"
#include "Ship.h"

using namespace nc;

void InitialScene::Load()
{
    //Light
    auto lvHandle = NcCreateEntity({-33.9f, 10.3f, -2.4f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    NcAddEngineComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = NcCreateEntity({30.0f, 20.0f, -20.0f}, {45.0f, -45.0f, 0.0f}, Vector3::Zero(), "Main Camera");
    auto camComponentPtr = NcAddUserComponent<Camera>(camHandle);
    NcRegisterMainCamera(camComponentPtr);
    NcAddUserComponent<CamController>(camHandle);
    
    //Debug Controller
    auto debugHandle = NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
    NcAddUserComponent<SceneReset>(debugHandle);
    NcAddUserComponent<Timer>(debugHandle);

    //GFX Objects
    // Meshes
    auto beeMesh = graphics::Mesh{"project//models//bee.fbx"};
    auto groundMesh = graphics::Mesh{"project//models//cube.fbx"};

    // Materials
    auto beeMaterial = graphics::PBRMaterial{"project//Textures//bee_albedo.png", "project//Textures//bee_normal.jpg",  "project//Textures//bee_roughness.png", "nc//source//graphics//DefaultTexture.png"};
    auto groundMaterial = graphics::PBRMaterial{"project//Textures//brickwall.jpg", "project//Textures//brickwall_normal.jpg", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"};
    groundMaterial.properties.xTiling = 10.0f;
    groundMaterial.properties.yTiling = 10.0f;
    groundMaterial.properties.color = {1.0f, 1.0f, 1.0f};
    auto wallMaterial = graphics::PBRMaterial{"project//Textures//wall_albedo.jpg", "project//Textures//wall_normal.jpg", "project//Textures//wall_specular.jpg", "nc//source//graphics//DefaultTexture.png"};
    wallMaterial.properties.xTiling = 10.0f;
    wallMaterial.properties.yTiling = 10.0f;

    //Ground
    auto groundHandle = NcCreateEntity({0.0f, -2.0f, 0.0f}, {}, {50.0f, 0.5f, 50.0f}, "Ground");
    NcAddEngineComponent<Renderer>(groundHandle, groundMesh, groundMaterial);

    //Ship
    auto shipHandle = NcCreateEntity(Vector3::Zero(), {1.5708f, 0.0f, 0.0f}, Vector3::One() * 10, "Ship");
    NcAddUserComponent<Ship>(shipHandle);
    NcAddEngineComponent<Renderer>(shipHandle, beeMesh, beeMaterial);

    // Wall
    auto wallHandle = NcCreateEntity({-48.5f, -2.0f, 0.0f}, {}, {0.5f, 50.0f, 50.0f}, "Wall");
    NcAddEngineComponent<Renderer>(wallHandle, groundMesh, wallMaterial);
}
 
void InitialScene::Unload()
{
    
}