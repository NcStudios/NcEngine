#include "InitialScene.h"
#include "NcCamera.h"
#include "component/Renderer.h"
#include "component/PointLight.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "CamController.h"
#include "DebugUtils.h"
#include "Ship.h"

using namespace nc;

void InitialScene::Load()
{
    // Light
    auto lvHandle = NcCreateEntity({-33.9f, 10.3f, -2.4f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    NcAddEngineComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = NcCreateEntity({0.0f, 80.0f, 0.0f}, {1.5f, 0.0f, 0.0f}, Vector3::Zero(), "Main Camera");
    auto camComponentPtr = NcAddUserComponent<Camera>(camHandle);
    NcRegisterMainCamera(camComponentPtr);
    NcAddUserComponent<CamController>(camHandle);
    
    // Debug Controller
    auto debugHandle = NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
    NcAddUserComponent<SceneReset>(debugHandle);
    NcAddUserComponent<Timer>(debugHandle);

    // Ground
    auto groundMaterial = graphics::PBRMaterial{{"project//Textures//brickwall.jpg", "project//Textures//brickwall_normal.jpg", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"}};
    groundMaterial.properties.xTiling = 10.0f;
    groundMaterial.properties.yTiling = 10.0f;
    groundMaterial.properties.color = {1.0f, 1.0f, 1.0f};
    auto groundHandle = NcCreateEntity({0.0f, -2.0f, 0.0f}, {}, {50.0f, 0.5f, 50.0f}, "Ground");
    auto groundMesh = graphics::Mesh{"project//Models//cube.fbx"};
    NcAddEngineComponent<Renderer>(groundHandle, groundMesh, groundMaterial);

    // Ship
    auto beeMaterial = graphics::PBRMaterial{{"project//Textures//bee_albedo.png", "project//Textures//bee_normal.jpg",  "project//Textures//bee_roughness.png", "nc//source//graphics//DefaultTexture.png"}};
    auto shipHandle = NcCreateEntity(Vector3::Zero(), {1.5708f, 0.0f, 0.0f}, Vector3::One() * 10, "Ship");
    NcAddUserComponent<Ship>(shipHandle);
    auto beeMesh = graphics::Mesh{"project//Models//bee.fbx"};
    NcAddEngineComponent<Renderer>(shipHandle, beeMesh, beeMaterial);

    // Wall
    auto wallMaterial = graphics::PBRMaterial{{"project//Textures//wall_albedo.jpg", "project//Textures//wall_normal.jpg", "project//Textures//wall_specular.jpg", "nc//source//graphics//DefaultTexture.png"}};
    wallMaterial.properties.xTiling = 10.0f;
    wallMaterial.properties.yTiling = 10.0f;
    auto wallHandle = NcCreateEntity({-48.5f, -2.0f, 0.0f}, {}, {0.5f, 50.0f, 50.0f}, "Wall");
    NcAddEngineComponent<Renderer>(wallHandle, groundMesh, wallMaterial);
}
 
void InitialScene::Unload()
{
    
}