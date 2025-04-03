#include "CubeMapBufferResource.h"
#include "ResourceTypes.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"

#include <algorithm>
#include <ranges>

namespace nc::graphics
{
using namespace Diligent;

CubeMapBufferResource::CubeMapBufferResource(IShaderResourceVariable& variable, uint32_t maxCubeMaps)
    : m_variable{&variable},
      m_maxCubeMaps{maxCubeMaps},
      m_initialLoadComplete{false}
{
}

auto CubeMapBufferResource::MakeSamplerDesc(std::string_view variableName) -> ImmutableSamplerDesc
{
    SamplerDesc samplerDesc{};
    samplerDesc.AddressU = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;

    return ImmutableSamplerDesc{
        SHADER_TYPE_PIXEL,
        variableName.data(),
        samplerDesc
    };
}

void CubeMapBufferResource::Load(std::span<const asset::CubeMapWithId> cubeMaps,
                              IDeviceContext& context,
                              IRenderDevice& device)
{
    const auto cubeMapCount = cubeMaps.size();
    if (cubeMapCount == 0)
    {
        return;
    }

    if (!m_initialLoadComplete)
    {
        InitializeCubeArray(context, device, m_variable, m_maxCubeMaps, false);
        m_initialLoadComplete = true;
    }

    if (cubeMapCount + m_cubeMaps.size() > m_maxCubeMaps)
    {
        throw NcError{"Max cube map count exceeded"};
    }

    auto barriers = std::vector<StateTransitionDesc>();
    barriers.reserve(cubeMapCount);
    m_cubeMaps.reserve(m_cubeMaps.size() + cubeMapCount);
    m_shaderResourceViews.reserve(m_shaderResourceViews.size() + cubeMapCount);

    for (const auto& [cubeMap, id] : cubeMaps)
    {
        const auto stride = cubeMap.faceSideLength * asset::CubeMap::numChannels;
        auto faceSubResources = std::array<Diligent::TextureSubResData, 6>{};
        std::ranges::transform(
            cubeMap.faces,
            faceSubResources.begin(),
            [stride](const std::vector<unsigned char>& face) {
                return TextureSubResData{face.data(), stride};
            }
        );

        auto texData = TextureData{faceSubResources.data(), 6, &context};
        auto desc = ToTextureCubeDesc(cubeMap);
        auto& cubeMapTextureHandle = m_cubeMaps.emplace_back();
        device.CreateTexture(desc, &texData, &cubeMapTextureHandle);
        if (!cubeMapTextureHandle)
        {
            throw NcError("Failed to create texture");
        }

        m_shaderResourceViews.push_back(cubeMapTextureHandle->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        barriers.emplace_back(
            cubeMapTextureHandle.RawPtr(),
            RESOURCE_STATE_UNKNOWN,
            RESOURCE_STATE_SHADER_RESOURCE,
            STATE_TRANSITION_FLAG_UPDATE_STATE
        );
    }

    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
    auto offset = m_shaderResourceViews.size() - cubeMapCount;
    SetArrayRegion(m_variable, std::span<IDeviceObject*>(m_shaderResourceViews.data() + offset, cubeMapCount), offset, cubeMapCount);
}

void CubeMapBufferResource::Unload()
{
    m_cubeMaps.clear();
    m_cubeMaps.shrink_to_fit();
    m_shaderResourceViews.clear();
    m_shaderResourceViews.shrink_to_fit();
}
} // namespace nc::graphics
