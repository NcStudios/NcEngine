#include "MenuScene.h"
#include "Ecs.h"
#include "NcCamera.h"
#include "NcUI.h"
#include "DebugUtils.h"
#include "CamController.h"
#include "project/components/CubeRotator.h"
#include "source/Prefabs.h"

#include <cstdlib>
#include <ctime>

using namespace nc;

void MenuScene::Load()
{
    m_ui = std::make_unique<project::ui::MainMenuUI>();
    ui::NcRegisterUI(m_ui.get());

    prefab::InitializeResources();
    
    // Light
    auto lvHandle = Ecs::CreateEntity({-33.9f, 10.3f, -2.4f}, Vector3::Zero(), Vector3::Zero(), "Point Light");
    Ecs::AddComponent<PointLight>(lvHandle);

    // CamController
    auto camHandle = Ecs::CreateEntity("Main Camera");
    auto camComponentPtr = Ecs::AddComponent<Camera>(camHandle);
    camera::NcRegisterMainCamera(camComponentPtr);
    
    // Debug Controller
    auto debugHandle = Ecs::CreateEntity("Debug Controller");
    Ecs::AddComponent<SceneReset>(debugHandle);
    Ecs::AddComponent<Timer>(debugHandle);

    auto ncMaterial = graphics::PBRMaterial{{"project//Textures//nc_logo.png", "nc//source//graphics//DefaultTexture_Normal.png", "nc//source//graphics//DefaultTexture.png", "nc//source//graphics//DefaultTexture.png"}};
    auto ncMesh = graphics::Mesh{"project//Models//cube.fbx"};
    std::srand(std::time(0));
    int posRange = 30;
    int rotRange = 90;
    for(unsigned i = 0; i < 40; ++i)
    {
        auto xPos = (float)(rand() % posRange) - (posRange / 2);
        auto yPos = (float)(rand() % posRange) - (posRange / 2);
        auto zPos = (float)(rand() % posRange) + (posRange / 2);
        auto xRot = (float)(rand() % rotRange);
        auto yRot = (float)(rand() % rotRange);
        auto zRot = (float)(rand() % rotRange);
        auto ncHandle = Ecs::CreateEntity({xPos, yPos, zPos}, {xRot, yRot, zRot}, {1.0f, 1.0f, 1.0f}, "nc");
        Ecs::AddComponent<Renderer>(ncHandle, ncMesh, ncMaterial);
        Ecs::AddComponent<project::CubeRotator>(ncHandle);
    }
}

void MenuScene::Unload()
{
    m_ui = nullptr;
    ui::NcRegisterUI(nullptr);
}