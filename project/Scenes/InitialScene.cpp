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

    //Spawner
    auto spawnHandle = NCE::CreateEntity({}, {}, {}, "Spawner");
    NCE::AddUserComponent<Spawner>(spawnHandle);
}
 
void InitialScene::Unload()
{
    
}