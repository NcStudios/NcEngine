#include "PostProcessColorSinkBufferResource.h"
#include "ncengine/asset/AssetData.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"
#include "fmt/format.h"

namespace nc::graphics
{
auto PostProcessColorSinkBufferResource::MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc
{
    return Diligent::ImmutableSamplerDesc{
        Diligent::SHADER_TYPE_PIXEL,
        variableName.data(),
        Diligent::SamplerDesc{}
    };
}

auto PostProcessColorSinkBufferResource::Add(Diligent::IRenderDevice& device,
                                             uint32_t numColorRenderTargets,
                                             uint32_t renderTargetWidth,
                                             uint32_t renderTargetHeight,
                                             uint32_t numSamples) -> std::vector<uint32_t>
{
    using namespace Diligent;

    auto addedIndices = std::vector<uint32_t>{};
    addedIndices.reserve(numColorRenderTargets);

    if (numColorRenderTargets == 0)
    {
        return addedIndices;
    }

    if (numSamples > 1)
    {
        if (numColorRenderTargets + m_colorRenderTargetsMsaa.size() > m_maxTextures)
        {
            throw NcError{"Max texture count exceeded"};
        }
        m_colorRenderTargetsMsaa.reserve(m_colorRenderTargetsMsaa.size() + numColorRenderTargets);
        m_colorRenderTargetViewsRTMsaa.reserve(m_colorRenderTargetViewsRTMsaa.size() + numColorRenderTargets);
        m_colorRenderTargetViewsSRMsaa.reserve(m_colorRenderTargetViewsSRMsaa.size() + numColorRenderTargets);
    }
    else
    {
        if (numColorRenderTargets + m_colorRenderTargets.size() > m_maxTextures)
        {
            throw NcError{"Max texture count exceeded"};
        }
        m_colorRenderTargets.reserve(m_colorRenderTargets.size() + numColorRenderTargets);
        m_colorRenderTargetViewsRT.reserve(m_colorRenderTargetViewsRT.size() + numColorRenderTargets);
        m_colorRenderTargetViewsSR.reserve(m_colorRenderTargetViewsSR.size() + numColorRenderTargets);
    }

    for (auto i = 0u; i < numColorRenderTargets; i++)
    {
        auto rtName = "Color Render Target: " + std::to_string(i);
        TextureDesc colorRenderTargetDesc;
        colorRenderTargetDesc.Name = rtName.data();
        colorRenderTargetDesc.Type = RESOURCE_DIM_TEX_2D;
        colorRenderTargetDesc.Width = renderTargetWidth;
        colorRenderTargetDesc.Height = renderTargetHeight;
        colorRenderTargetDesc.MipLevels = 1;
        colorRenderTargetDesc.Format = TEX_FORMAT_RGBA8_UNORM;
        colorRenderTargetDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
        colorRenderTargetDesc.ClearValue.Format = colorRenderTargetDesc.Format;
        colorRenderTargetDesc.ClearValue.Color[0] = 0.350f;
        colorRenderTargetDesc.ClearValue.Color[1] = 0.350f;
        colorRenderTargetDesc.ClearValue.Color[2] = 0.350f;
        colorRenderTargetDesc.ClearValue.Color[3] = 1.0f;
        colorRenderTargetDesc.SampleCount = numSamples;

        RefCntAutoPtr<ITexture> pColorRenderTarget;
        device.CreateTexture(colorRenderTargetDesc, nullptr, &pColorRenderTarget);
        if (!pColorRenderTarget)
        {
            throw NcError("Failed to create texture");
        }

        if (numSamples > 1)
        {
            addedIndices.push_back(static_cast<uint32_t>(m_colorRenderTargetsMsaa.size()));
            m_colorRenderTargetsMsaa.push_back(std::move(pColorRenderTarget));
            m_colorRenderTargetViewsRTMsaa.push_back(m_colorRenderTargetsMsaa.back()->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
            m_colorRenderTargetViewsSRMsaa.push_back(m_colorRenderTargetsMsaa.back()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        }
        else
        {
            addedIndices.push_back(static_cast<uint32_t>(m_colorRenderTargets.size()));
            m_colorRenderTargets.push_back(std::move(pColorRenderTarget));
            m_colorRenderTargetViewsRT.push_back(m_colorRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
            m_colorRenderTargetViewsSR.push_back(m_colorRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        }
    }

    if (numSamples > 1)
    {
        SetArrayRegion(m_colorRenderTargetViewsSRMsaa, m_colorRenderTargetViewsSR.size(), m_colorRenderTargetViewsSRMsaa.size());
    }
    else
    {
        SetArrayRegion(m_colorRenderTargetViewsSR, 0u, m_colorRenderTargetViewsSR.size());
    }
    return addedIndices;
}

void PostProcessColorSinkBufferResource::Resize(Diligent::IRenderDevice& device,
                                       uint32_t renderTargetWidth,
                                       uint32_t renderTargetHeight,
                                       uint32_t numSamples)
{
    auto numColorRenderTargets = static_cast<uint32_t>(m_colorRenderTargets.size());
    auto numColorRenderTargetsMsaa = static_cast<uint32_t>(m_colorRenderTargetsMsaa.size());
    Clear();
    Add(device, numColorRenderTargets, renderTargetWidth, renderTargetHeight, 1);
    if (numSamples > 1)
    {
        Add(device, numColorRenderTargetsMsaa, renderTargetWidth, renderTargetHeight, numSamples);
    }
}

void PostProcessColorSinkBufferResource::Clear()
{
    m_colorRenderTargets.clear();
    m_colorRenderTargets.shrink_to_fit();
    m_colorRenderTargetViewsRT.clear();
    m_colorRenderTargetViewsRT.shrink_to_fit();
    m_colorRenderTargetViewsSR.clear();
    m_colorRenderTargetViewsSR.shrink_to_fit();

    m_colorRenderTargetsMsaa.clear();
    m_colorRenderTargetsMsaa.shrink_to_fit();
    m_colorRenderTargetViewsRTMsaa.clear();
    m_colorRenderTargetViewsRTMsaa.shrink_to_fit();
    m_colorRenderTargetViewsSRMsaa.clear();
    m_colorRenderTargetViewsSRMsaa.shrink_to_fit();
}

void PostProcessColorSinkBufferResource::SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& views, size_t offset, size_t count)
{
    m_variable->SetArray(
        views.data(),
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
