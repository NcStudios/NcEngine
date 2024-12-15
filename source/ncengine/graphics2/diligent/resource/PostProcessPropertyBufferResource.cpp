#include "PostProcessPropertyBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <concepts>
#include <ranges>

namespace nc::graphics
{
PostProcessPropertyBufferResource::PostProcessPropertyBufferResource(Diligent::IDeviceContext& context,
                                                                     Diligent::IRenderDevice& device,
                                                                     Diligent::IShaderResourceVariable& outlinePassVariable)

    : m_outlinePassBuffer{
        context,
        device,
        OutlinePassData{},
        "OutlinePassDataUniformBuffer"
      },
      m_outlinePassVariable{&outlinePassVariable}
{
    m_outlinePassVariable->Set(&m_outlinePassBuffer.GetBuffer());
}

void PostProcessPropertyBufferResource::Update(Diligent::IDeviceContext& context,
                                               const PostProcessPassProperties& properties)
{
    std::visit(
        [&context, this](auto&& unpacked){
            using T = std::decay_t<decltype(unpacked)>;
            if constexpr (std::same_as<T, OutlinePassProperties>)
            {
                m_outlinePassBuffer.Write(context, unpacked);
            }
            else
            {
                NC_ASSERT(false, "Unhandled PostProcessPassProperties inner type");
            }
        },
        properties
    );
}
} // namespace nc::graphics
