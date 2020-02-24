#include "Bindable.h"

namespace nc::graphics::internal
{
    ID3D11DeviceContext* Bindable::GetContext(Graphics& graphics) noexcept
    {
        return graphics.m_context.Get();
    }

    ID3D11Device* Bindable::GetDevice(Graphics& graphics) noexcept
    {
        return graphics.m_device.Get();
    }
}