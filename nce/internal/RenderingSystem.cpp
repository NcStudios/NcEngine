#include "RenderingSystem.h"
#include "Graphics.h"
#include "Transform.h"
#include "ProjectSettings.h"
#include "NCTime.h"

#include "Box.h"
#include "Drawable.h"

#include "Camera.h"
#include "PointLight.h"
#include "EditorManager.h"
#include "Input.h"


namespace nc::internal{

RenderingSystem::RenderingSystem(int initialWidth, int initialHeight, HWND hwnd)
    : m_camera(std::make_unique<Camera>()), 
      m_graphics(std::make_unique<graphics::internal::Graphics>(hwnd, initialWidth, initialHeight))
{
    std::mt19937 rng( std::random_device{}() );
	std::uniform_real_distribution<float> adist(0.0f,3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f,3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f,3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f,20.0f);
    std::uniform_real_distribution<float> cdist(0.0f, 1.0f); 

    //make cubes
    for(auto i = 0; i < 70; ++i)
    {
        const DirectX::XMFLOAT3 materialColor = {cdist(rng), cdist(rng), cdist(rng)};
        m_drawables.push_back(std::make_unique<nc::graphics::primitive::Box>(GetGraphics(),
                                                                             rng, adist, ddist, odist, rdist,
                                                                             materialColor));
    }

    //make light
    m_pointLight = std::make_unique<graphics::PointLight>(GetGraphics());

    m_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)initialHeight / (float)initialWidth, 0.2f, 80.0f));
}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::StartRenderCycle(const std::vector<Transform> &transforms)
{
    static float speed_factor = 1.0f;

    if(input::GetKeyUp(input::KeyCode::Tilde))
    {
        m_editorManager->ToggleGui();
    }
    
    m_graphics->SetCamera(m_camera->GetMatrix());

    m_pointLight->Bind(GetGraphics(), m_camera->GetMatrix());

    m_editorManager->BeginFrame();
    BeginFrame();

    auto dt = nc::time::Time::FrameDeltaTime;

    for(auto& c : m_drawables)
    {
        c->Update(dt * speed_factor / 2.0f);
        c->Draw(GetGraphics());
    }

    if(m_editorManager->IsGuiActive())
    {
        m_editorManager->SpeedControl(&speed_factor);
        editor::EditorManager::CameraControl(&m_camera->zPos, &m_camera->phi, &m_camera->theta, &m_camera->xRot, &m_camera->yRot, &m_camera->zRot);
        m_pointLight->SpawnControlWindow();
    }

    m_editorManager->EndFrame();
    EndFrame();
}

nc::graphics::internal::Graphics& RenderingSystem::GetGraphics()
{
    return *m_graphics;
}

void RenderingSystem::BindEditorManager(editor::EditorManager* editorManager)
{
    m_editorManager = editorManager;
}

void RenderingSystem::BeginFrame()
{
    m_graphics->ClearBuffer(0.0f, 0.0f, 0.0f);
}

void RenderingSystem::EndFrame()
{
    m_graphics->EndFrame();
}

} //end namespace nc::internal