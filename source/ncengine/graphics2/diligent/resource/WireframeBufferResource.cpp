#include "WireframeBufferResource.h"

namespace nc::graphics
{
WireframeBufferResource::WireframeBufferResource(Diligent::IDeviceContext& context,
                                                 Diligent::IRenderDevice& device,
                                                 Diligent::IShaderResourceVariable& variable)
    : m_buffer{
        context,
        device,
        WireframeData{},
        "WireframeDataUniformBuffer"
      },
      m_variable{&variable}
{
    m_variable->Set(&m_buffer.GetBuffer());
}

void WireframeBufferResource::Update(Diligent::IDeviceContext& context,
                                     const WireframeData& data)
{
    m_buffer.Write(context, data);
}
} // namespace nc::graphics
