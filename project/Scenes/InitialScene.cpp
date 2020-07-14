#include "InitialScene.h"
#include "component/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "component/PointLight.h"
#include "component/Camera.h"

#include "CamController.h"
#include "DebugUtils.h"
#include "Ship.h"

using namespace nc;

void InitialScene::Load()
{
    //Light
    auto lvHandle = NCE::CreateEntity({0.0f, 3.0f, 0.0f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    NCE::AddEngineComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = NCE::CreateEntity({30.0f, 20.0f, -20.0f}, {45.0f, -45.0f, 0.0f}, Vector3::Zero(), "Main Camera");
    auto camComponentPtr = NCE::AddUserComponent<Camera>(camHandle);
    NCE::RegisterMainCamera(camComponentPtr);
    NCE::AddUserComponent<CamController>(camHandle);
    
    //Debug Controller
    auto debugHandle = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "Debug Controller");
    NCE::AddUserComponent<SceneReset>(debugHandle);
    NCE::AddUserComponent<Timer>(debugHandle);

    //GFX Objects
    auto beeMesh = graphics::Mesh{"project//models//bee.fbx"};
    auto groundMesh = graphics::Mesh{"project//models//cube.fbx"};
    auto beeMaterial = graphics::PBRMaterial{{1.0f, 1.0f, 1.0f}, "project//Textures//bee.png", "project//Textures//Tatami_normal.png",  "nc//graphics//DefaultTexture.png", "nc//graphics//DefaultTexture.png"};
    auto groundMaterial = graphics::PBRMaterial{{0.5f, 1.0f, 1.0f}, "project//Textures//tatami_basecolor.png", "nc//graphics//DefaultTexture.png", "nc//graphics//DefaultTexture.png", "nc//graphics//DefaultTexture.png"};

    //Ground
    auto groundHandle = NCE::CreateEntity({0.0f, -2.0f, 0.0f}, {}, {50.0f, 0.25f, 50.0f}, "Ground");
    NCE::AddEngineComponent<Renderer>(groundHandle, groundMesh, groundMaterial);

    //Ship
    auto shipHandle = NCE::CreateEntity(Vector3::Zero(), {1.5708f, 0.0f, 0.0f}, Vector3::One() * 10, "Ship");
    NCE::AddUserComponent<Ship>(shipHandle);
    NCE::AddEngineComponent<Renderer>(shipHandle, beeMesh, beeMaterial);
}
 
void InitialScene::Unload()
{
    
}