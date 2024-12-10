#include "PostProcessSinkIndexBufferResource.h"

#include "ncutility/NcError.h"

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

void PostProcessSinkIndexBufferResource::Update(Diligent::IDeviceContext& context, const SinkTargets& sinkTargets)
{
    const auto data = PostProcessSinkIndexData
    {
        sinkTargets.color[0], sinkTargets.color[1], sinkTargets.color[2], sinkTargets.color[3],
        sinkTargets.depth[0], sinkTargets.depth[1], sinkTargets.depth[2], sinkTargets.depth[3],
    };
    m_buffer.Write(context, data);
}
} // namespace nc::graphics
