#include "GraphicsResource.h"
#include "Graphics.h"

namespace nc::graphics::d3dresource
{
    ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& graphics) noexcept
    {
        return graphics.m_context.Get();
    }

    ID3D11Device* GraphicsResource::GetDevice(Graphics& graphics) noexcept
    {
        return graphics.m_device.Get();
    }
}