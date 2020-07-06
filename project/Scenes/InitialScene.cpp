#include "InitialScene.h"

//#include <random>
#include "component/Renderer.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "component/PointLight.h"
#include "Timer.h"
#include "Spawner.h"

void InitialScene::Load()
{
    //add light
    auto lvHandle = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "PointLight");
    NCE::AddEngineComponent<PointLight>(lvHandle);

    //CamController
    auto camHandle = NCE::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), "CameraController");
    NCE::AddUserComponent<CamController>(camHandle);

    //Timer
    auto timerHandle = NCE::CreateEntity({}, {}, {}, "Timer");
    NCE::AddUserComponent<Timer>(timerHandle);

    //Floor
    Vector3  position{ -15.0f, 0.0f, 0.0f };
    Vector3  scale{ 50.0f, 50.0f, 50.0f };
    Vector3  rotation{ -53.0f, 83.0f, -52.0f };
    EntityHandle handle = NCE::CreateEntity(position, rotation, scale, "Square");
    NCE::AddUserComponent<Head>(handle);
    auto mesh = nc::graphics::Mesh("project\\models\\plane.obj");
    auto rend = NCE::AddEngineComponent<Renderer>(handle, mesh);
    auto pbrMaterial = nc::graphics::PBRMaterial();
    rend->SetMaterial(pbrMaterial);
}
 
void InitialScene::Unload()
{
    
}