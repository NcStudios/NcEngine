#include "PostProcessSinkIndexBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

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

void PostProcessSinkIndexBufferResource::Update(Diligent::IDeviceContext& context, std::span<const uint32_t> colorSources, std::span<const uint32_t> depthSources)
{
    NC_ASSERT(colorSources.size() <= 4u, "Only four color sources supported.");
    NC_ASSERT(depthSources.size() <= 4u, "Only four depth sources supported.");

    auto colorSourcesArray = std::array<uint32_t, 4u>{0u};
    auto depthSourcesArray = std::array<uint32_t, 4u>{0u};

    for (auto i = 0u; i < colorSources.size(); i++)
    {
        colorSourcesArray.at(i) = colorSources[i];
    }

    for (auto i = 0u; i < depthSources.size(); i++)
    {
        depthSourcesArray.at(i) = depthSources[i];
    }

    const auto data = PostProcessSinkIndexData
    {
        colorSourcesArray[0], colorSourcesArray[1], colorSourcesArray[2], colorSourcesArray[3],
        depthSourcesArray[0], depthSourcesArray[1], depthSourcesArray[2], depthSourcesArray[3],
    };
    m_buffer.Write(context, data);
}
} // namespace nc::graphics
