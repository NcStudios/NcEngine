#include "PostProcessSinkIndexBufferResource.h"

namespace nc::graphics
{
PostProcessSinkIndexBufferResource::PostProcessSinkIndexBufferResource(Diligent::IDeviceContext& context,
                                                                       Diligent::IRenderDevice& device,
                                                                       Diligent::IShaderResourceVariable& variable)
    : m_buffer{
        context,
        device,
        PostProcessSinkIndexData{},
        UniformBufferName},
      m_variable{&variable}
{
    m_variable->Set(&m_buffer.GetBuffer());
}

void PostProcessSinkIndexBufferResource::Update(Diligent::IDeviceContext& context, uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex)
{
    const auto data = PostProcessSinkIndexData{colorRenderTargetIndex, depthRenderTargetIndex};
    m_buffer.Write(context, data);
}
} // namespace nc::graphics
