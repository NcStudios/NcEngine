#include "InitialScene.h"
#include "component/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
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
    auto wormMesh = graphics::Mesh{"project//models//defaultMesh.obj"};
    auto cubeMesh = graphics::Mesh{"project//models//cube.obj"};
    auto shipMaterial = graphics::PBRMaterial{};
    auto groundMaterial = graphics::PBRMaterial{};

    //Ground
    auto groundHandle = NCE::CreateEntity({0.0f, -2.0f, 0.0f}, {}, {50.0f, 0.25f, 50.0f}, "Ground");
    auto groundRenderer = NCE::AddEngineComponent<Renderer>(groundHandle, cubeMesh);
    groundRenderer->SetMaterial(groundMaterial);

    //Floor
    Vector3  position{ -15.0f, 0.0f, 0.0f };
    Vector3  scale{ 50.0f, 50.0f, 50.0f };
    Vector3  rotation{ -53.0f, 83.0f, -52.0f };
    EntityHandle handle = NCE::CreateEntity(position, rotation, scale, "Square");
    NCE::AddUserComponent<Ship>(handle);
    auto mesh = nc::graphics::Mesh("project\\models\\plane.obj");
    auto rend = NCE::AddEngineComponent<Renderer>(handle, mesh);
    auto pbrMaterial = nc::graphics::PBRMaterial();
    rend->SetMaterial(pbrMaterial);

    //Ship
    auto shipHandle = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), "Ship");
    NCE::AddUserComponent<Ship>(shipHandle);
    auto shipRend = NCE::AddEngineComponent<Renderer>(shipHandle, wormMesh);
    shipRend->SetMaterial(shipMaterial);
}
 
void InitialScene::Unload()
{
    
}