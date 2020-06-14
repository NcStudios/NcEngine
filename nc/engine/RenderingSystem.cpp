#include "RenderingSystem.h"
#include "NCE.h"
#include "graphics/Graphics.h"

#include "component/Camera.h"
#include "component/PointLight.h"

namespace nc::engine{

RenderingSystem::RenderingSystem(int initialWidth, int initialHeight, HWND hwnd)
    : m_graphics(std::make_unique<graphics::Graphics>(hwnd, initialWidth, initialHeight))
{
    graphics::d3dresource::GraphicsResourceManager::SetGraphics(m_graphics.get());
    m_graphics->SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, (float)initialHeight / (float)initialWidth, 0.5f, 400.0f));
}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::FrameBegin()
{
    auto camMatrixXM = NCE::GetTransform(*NCE::GetMainCamera())->CamGetMatrix();
    m_graphics->StartFrame();
    m_graphics->SetCamera(camMatrixXM);
}

void RenderingSystem::Frame()
{
    auto gfx = m_graphics.get();

    ComponentSystem<Renderer>::ForEach([&gfx](auto & rend)
    {
        rend.Update(gfx);
    });
}

void RenderingSystem::FrameEnd()
{
    m_graphics->EndFrame();
}

nc::graphics::Graphics* RenderingSystem::GetGraphics()
{
    return m_graphics.get();
}

} //end namespace nc::internal