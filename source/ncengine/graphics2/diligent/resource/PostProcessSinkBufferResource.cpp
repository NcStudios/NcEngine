#include "PostProcessSinkBufferResource.h"
#include "ncengine/asset/AssetData.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"
#include "fmt/format.h"

namespace nc::graphics
{
auto PostProcessSinkBufferResource::MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc
{
    return Diligent::ImmutableSamplerDesc{
        Diligent::SHADER_TYPE_PIXEL,
        variableName.data(),
        Diligent::SamplerDesc{}
    };
}

auto PostProcessSinkBufferResource::Add(Diligent::IRenderDevice& device,
                                    uint32_t numColorRenderTargets,
                                    uint32_t numDepthRenderTargets,
                                    uint32_t renderTargetWidth,
                                    uint32_t renderTargetHeight) -> std::vector<uint32_t>
{
    using namespace Diligent;

    auto addedIndices = std::vector<uint32_t>{};
    addedIndices.reserve(numColorRenderTargets + numDepthRenderTargets);

    const auto numRenderTargets = numColorRenderTargets + numDepthRenderTargets;
    if (numRenderTargets == 0)
    {
        return addedIndices;
    }

    if (numRenderTargets + m_colorRenderTargets.size() + m_depthRenderTargets.size() > m_maxTextures)
    {
        throw NcError{"Max texture count exceeded"};
    }

    m_colorRenderTargets.reserve(m_colorRenderTargets.size() + numColorRenderTargets);
    m_depthRenderTargets.reserve(m_depthRenderTargets.size() + numDepthRenderTargets);
    m_colorRenderTargetViewsRT.reserve(m_colorRenderTargetViewsRT.size() + numColorRenderTargets);
    m_colorRenderTargetViewsSR.reserve(m_colorRenderTargetViewsSR.size() + numColorRenderTargets);
    m_depthRenderTargetViewsRT.reserve(m_depthRenderTargetViewsRT.size() + numDepthRenderTargets);

    for (auto i = 0u; i < numColorRenderTargets; i++)
    {
        TextureDesc colorRenderTargetDesc;
        colorRenderTargetDesc.Name = "Color Render Target"; //fmt::format("ColRT {0}", m_colorRenderTargets.size() + i).c_str();
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

        RefCntAutoPtr<ITexture> pColorRenderTarget;
        device.CreateTexture(colorRenderTargetDesc, nullptr, &pColorRenderTarget);
        if (!pColorRenderTarget)
        {
            throw NcError("Failed to create texture");
        }

        addedIndices.push_back(static_cast<uint32_t>(m_colorRenderTargets.size()));
        m_colorRenderTargets.push_back(std::move(pColorRenderTarget));
        m_colorRenderTargetViewsRT.push_back(m_colorRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
        m_colorRenderTargetViewsSR.push_back(m_colorRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
    }

    for (auto i = 0u; i < numDepthRenderTargets; i++)
    {
        TextureDesc depthRenderTargetDesc;
        depthRenderTargetDesc.Name = "Depth Render Target"; // fmt::format("DepthRT {0}", m_depthRenderTargets.size() + i).c_str();
        depthRenderTargetDesc.Type = RESOURCE_DIM_TEX_2D;
        depthRenderTargetDesc.Width = renderTargetWidth;
        depthRenderTargetDesc.Height = renderTargetHeight;
        depthRenderTargetDesc.MipLevels = 1;
        depthRenderTargetDesc.Format = TEX_FORMAT_D32_FLOAT;
        depthRenderTargetDesc.BindFlags = BIND_DEPTH_STENCIL;
        depthRenderTargetDesc.ClearValue.Format = depthRenderTargetDesc.Format;
        depthRenderTargetDesc.ClearValue.DepthStencil.Depth = 1;
        depthRenderTargetDesc.ClearValue.DepthStencil.Stencil = 0;

        RefCntAutoPtr<ITexture> pDepthRenderTarget;
        device.CreateTexture(depthRenderTargetDesc, nullptr, &pDepthRenderTarget);
        if (!pDepthRenderTarget)
        {
            throw NcError("Failed to create texture");
        }

        addedIndices.push_back(static_cast<uint32_t>(m_depthRenderTargets.size()));
        m_depthRenderTargets.push_back(std::move(pDepthRenderTarget));
        m_depthRenderTargetViewsRT.push_back(m_depthRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL));
    }

    // auto combinedViews = std::vector<Diligent::IDeviceObject*>{};
    // combinedViews.reserve(m_depthRenderTargetViewsRT.size() + m_colorRenderTargetViews.size());
    // combinedViews.append_range(m_colorRenderTargetViewsSR);
    // combinedViews.append_range(m_depthRenderTargetViewsRT);

    // if (numColorRenderTargets == 0)
    // {
    //     // If no color render targets were added, just update the depth render targets that have changed
    //     SetArrayRegion(combinedViews, combinedViews.size() - numDepthRenderTargets, numDepthRenderTargets);
    //     return;
    // }

    // // If color and depth were added, or just color was added, 
    // // then remap all of the depth targets (always) and the color targets that were added
    // auto count = numColorRenderTargets + m_depthRenderTargetViewsRT.size();
    SetArrayRegion(m_colorRenderTargetViewsSR, m_colorRenderTargetViewsSR.size() - numColorRenderTargets, numColorRenderTargets);
    return addedIndices;
}

void PostProcessSinkBufferResource::Resize(Diligent::IRenderDevice& device,
                                       uint32_t renderTargetWidth,
                                       uint32_t renderTargetHeight)
{
    auto numColorRenderTargets = static_cast<uint32_t>(m_colorRenderTargets.size());
    auto numDepthRenderTargets = static_cast<uint32_t>(m_depthRenderTargets.size());
    Clear();
    Add(device, numColorRenderTargets, numDepthRenderTargets, renderTargetWidth, renderTargetHeight);
}

void PostProcessSinkBufferResource::Clear()
{
    m_colorRenderTargets.clear();
    m_colorRenderTargets.shrink_to_fit();
    m_colorRenderTargetViewsRT.clear();
    m_colorRenderTargetViewsRT.shrink_to_fit();
    m_colorRenderTargetViewsSR.clear();
    m_colorRenderTargetViewsSR.shrink_to_fit();
    m_depthRenderTargets.clear();
    m_depthRenderTargets.shrink_to_fit();
    m_depthRenderTargetViewsRT.clear();
    m_depthRenderTargetViewsRT.shrink_to_fit();
}

void PostProcessSinkBufferResource::SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& views, size_t offset, size_t count)
{
    m_variable->SetArray(
        views.data() + offset,
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
