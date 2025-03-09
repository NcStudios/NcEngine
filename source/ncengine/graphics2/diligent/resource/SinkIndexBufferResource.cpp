#include "SinkIndexBufferResource.h"
#include "graphics2/diligent/pass/PassTypes.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
SinkIndexBufferResource::SinkIndexBufferResource(Diligent::IDeviceContext& context,
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

void SinkIndexBufferResource::Update(Diligent::IDeviceContext& context,
                                     std::span<const uint32_t> colorSources,
                                     std::span<const uint32_t> depthSources,
                                     bool hasPostProcessSource,
                                     uint32_t lightIndex,
                                     uint32_t lightFaceIndex)
{
    NC_ASSERT(colorSources.size() <= 4u, "Only four color sources supported.");
    NC_ASSERT(depthSources.size() <= 3u, "Only three depth sources supported.");

    auto colorSourcesArray = std::array<int32_t, 4u>{-1};
    auto depthSourcesArray = std::array<int32_t, 3u>{-1};

    for (auto i = 0u; i < colorSources.size(); i++)
    {
        if (colorSources[i] != SwapChainTarget && colorSources[i] != NoTarget)
        {
            colorSourcesArray.at(i) = colorSources[i];
        }
    }

    for (auto i = 0u; i < depthSources.size(); i++)
    {
        if (depthSources[i] != DepthStencilTarget && depthSources[i] != NoTarget)
        {
            depthSourcesArray.at(i) = depthSources[i];
        }
    }

    const auto data = PostProcessSinkIndexData
    {
        colorSourcesArray[0], colorSourcesArray[1], colorSourcesArray[2], colorSourcesArray[3],
        depthSourcesArray[0], depthSourcesArray[1], depthSourcesArray[2], hasPostProcessSource ? 1u : 0u,
        lightIndex, lightFaceIndex
    };
    m_buffer.Write(context, data);
}
} // namespace nc::graphics
