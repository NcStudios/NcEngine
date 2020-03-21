#include "RenderingSystem.h"
#include "NCE.h"
#include "Graphics.h"

#include "Camera.h"
#include "PointLight.h"

namespace nc::engine{

RenderingSystem::RenderingSystem(int initialWidth, int initialHeight, HWND hwnd)
    : m_graphics(std::make_unique<graphics::Graphics>(hwnd, initialWidth, initialHeight))
{
    //m_pointLight = std::make_unique<PointLight>(0, EntityView());
    //m_pointLight->Set(GetGraphics(), DirectX::XMFLOAT3(0,0,0));

    m_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)initialHeight / (float)initialWidth, 0.2f, 80.0f));
}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::FrameBegin()
{
    auto camMatrixXM = NCE::GetMainCamera()->Transform()->CamGetMatrix();
    m_graphics->SetCamera(camMatrixXM);

    // for(auto& light : m_pointLights)
    // {
    //     light->Bind(GetGraphics(), camMatrixXM);
    // }

    m_graphics->ClearBuffer(0.0f, 0.0f, 0.0f);
}

void RenderingSystem::Frame()
{
    auto& graphics = GetGraphics();

    for(auto& r : m_components)
    {
        r.Update(graphics);
    }
}

void RenderingSystem::FrameEnd()
{
    m_graphics->EndFrame();
}

nc::graphics::Graphics& RenderingSystem::GetGraphics()
{
    return *m_graphics;
}

} //end namespace nc::internal