#include "RenderingSystem.h"
#include "NcCamera.h"
#include "graphics/Graphics.h"
#include "component/PointLight.h"
#include "component/Transform.h"

namespace nc::engine::system
{

RenderingSystem::RenderingSystem(HWND hwnd, float width, float height, float nearZ, float farZ, bool fullscreen)
    : m_graphics(std::make_unique<graphics::Graphics>(hwnd, width, height, nearZ, farZ, fullscreen))
{
    graphics::d3dresource::GraphicsResourceManager::SetGraphics(m_graphics.get());
}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::FrameBegin()
{
    auto camT = NcGetMainCameraTransform();
    auto camMatrixXM = camT->CamGetMatrix();
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