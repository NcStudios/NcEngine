#include "PostProcessPropertyBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <concepts>
#include <ranges>

namespace
{
auto ToNoisePassData(const nc::NoisePassProperties& properties) -> nc::graphics::NoisePassData
{
    return nc::graphics::NoisePassData{
        .maskGradientStart = properties.maskGradientStart,
        .maskGradientAmount = properties.maskGradientAmount,
        .maskGradientEnd = properties.maskGradientEnd,
        .noiseTexIndex = properties.noiseTex.index,
        .noiseTexAmount = properties.noiseTexAmount,
        .noiseTexTiling = properties.noiseTexTiling
    };
}
} // anonymous namespace

namespace nc::graphics
{
PostProcessPropertyBufferResource::PostProcessPropertyBufferResource(Diligent::IDeviceContext& context,
                                                                     Diligent::IRenderDevice& device,
                                                                     Diligent::IShaderResourceVariable& postProcessPassVariable)

    : m_outlinePassBuffer{
        context,
        device,
        OutlinePassData{},
        "OutlinePassDataUniformBuffer"
      },
      m_noisePassBuffer{
        context,
        device,
        NoisePassData{},
        "NoisePassDataUniformBuffer"
      },
      m_postProcessPassVariable{&postProcessPassVariable}
{
}

void PostProcessPropertyBufferResource::Update(Diligent::IDeviceContext& context,
                                               const PostProcessPassProperties& properties)
{
    std::visit(
        [&context, this](auto&& unpacked) {
            using T = std::decay_t<decltype(unpacked)>;
            if constexpr (std::same_as<T, OutlinePassProperties>)
            {

                m_postProcessPassVariable->Set(&m_outlinePassBuffer.GetBuffer());
                m_outlinePassBuffer.Write(context, unpacked);
            }
            else if constexpr (std::same_as<T, NoisePassProperties>)
            {
                m_postProcessPassVariable->Set(&m_noisePassBuffer.GetBuffer());
                m_noisePassBuffer.Write(context, ToNoisePassData(unpacked));
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
