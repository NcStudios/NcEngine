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
    auto lvHandle = NcCreateEntity({0.0f, 3.0f, 0.0f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
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
    auto beeMesh = graphics::Mesh{"project//models//bee.fbx"};
    auto groundMesh = graphics::Mesh{"project//models//cube.fbx"};
    auto beeMaterial = graphics::PBRMaterial{{1.0f, 1.0f, 1.0f}, "project//Textures//bee.png"};
    auto groundMaterial = graphics::PBRMaterial{{1.0f, 1.0f, 1.0f}, "project//Textures//ground.jpg"};

    //Ground
    auto groundHandle = NcCreateEntity({0.0f, -2.0f, 0.0f}, {}, {50.0f, 0.25f, 50.0f}, "Ground");
    NcAddEngineComponent<Renderer>(groundHandle, groundMesh, groundMaterial);

    //Ship
    auto shipHandle = NcCreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One() * 10, "Ship");
    NcAddUserComponent<Ship>(shipHandle);
    NcAddEngineComponent<Renderer>(shipHandle, beeMesh, beeMaterial);
}
 
void InitialScene::Unload()
{
    
}