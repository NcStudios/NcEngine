#include "Graphics2.h"
#include "vulkan/Device.h"
#include "vulkan/Instance.h"
#include "vulkan/GraphicsPipeline.h"
#include "vulkan/RenderPass.h"

namespace nc::graphics
{
    using namespace vulkan;

    Graphics2::Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions)
    : m_instance{nullptr},
      m_device{nullptr},
      m_pipeline{nullptr},
      m_renderPass{nullptr},
      m_isFullscreen {false},
      m_viewMatrix{},
      m_projectionMatrix{}
    {
        m_instance = std::make_unique<Instance>(hwnd, hinstance);
        m_device = std::make_unique<Device>(m_instance.get(), dimensions);
        m_renderPass = std::make_unique<RenderPass>(m_device.get());
        m_pipeline = std::make_unique<GraphicsPipeline>(m_device.get(), m_renderPass.get());
    }

    Graphics2::~Graphics2() = default;

    DirectX::FXMMATRIX Graphics2::GetViewMatrix() const noexcept
    {
        return m_viewMatrix;
    }

    DirectX::FXMMATRIX Graphics2::GetProjectionMatrix() const noexcept
    {
        return m_projectionMatrix;
    }
    
    void Graphics2::SetViewMatrix(DirectX::FXMMATRIX cam) noexcept
    {
        m_viewMatrix = cam;
    }

    void Graphics2::SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept
    {
        m_projectionMatrix = DirectX::XMMatrixPerspectiveLH(1.0f, height / width, nearZ, farZ);
    }

    void Graphics2::ToggleFullscreen()
    {
        // @todo
    }

    void Graphics2::ResizeTarget(float width, float height)
    {
        (void)width;
        (void)height;
        // @todo
    }

    void Graphics2::OnResize(float width, float height, float nearZ, float farZ)
    {    
        (void)width;
        (void)height;
        (void)nearZ;
        (void)farZ;
        // @todo
    }

    void Graphics2::FrameBegin()
    {
        // @todo
    }

    void Graphics2::DrawIndexed(UINT count)
    {
        (void)count;
        // @todo
    }

    void Graphics2::FrameEnd()
    {
        // @todo
    }
}