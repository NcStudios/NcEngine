#include "PostProcessDepthSinkBufferResource.h"
#include "ncengine/asset/AssetData.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"
#include "fmt/format.h"

namespace nc::graphics
{
auto PostProcessDepthSinkBufferResource::Add(Diligent::IRenderDevice& device,
                                             uint32_t numDepthRenderTargets,
                                             uint32_t renderTargetWidth,
                                             uint32_t renderTargetHeight) -> std::vector<uint32_t>
{
    using namespace Diligent;

    auto addedIndices = std::vector<uint32_t>{};
    addedIndices.reserve(numDepthRenderTargets);

    if (numDepthRenderTargets == 0)
    {
        return addedIndices;
    }

    if (numDepthRenderTargets + m_depthRenderTargets.size() > m_maxTextures)
    {
        throw NcError{"Max texture count exceeded"};
    }

    m_depthRenderTargets.reserve(m_depthRenderTargets.size() + numDepthRenderTargets);
    m_depthRenderTargetViewsRT.reserve(m_depthRenderTargetViewsRT.size() + numDepthRenderTargets);
    m_depthRenderTargetViewsSR.reserve(m_depthRenderTargetViewsSR.size() + numDepthRenderTargets);

    for (auto i = 0u; i < numDepthRenderTargets; i++)
    {
        auto rtName = "Depth Render Target: " + std::to_string(i);
        TextureDesc depthRenderTargetDesc;
        depthRenderTargetDesc.Name = rtName.data();
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
        m_depthRenderTargetViewsSR.push_back(m_depthRenderTargets.back()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
    }

    SetArrayRegion(m_depthRenderTargetViewsSR, 0u, m_depthRenderTargetViewsSR.size());
    return addedIndices;
}

void PostProcessDepthSinkBufferResource::Resize(Diligent::IRenderDevice& device,
                                       uint32_t renderTargetWidth,
                                       uint32_t renderTargetHeight)
{
    auto numDepthRenderTargets = static_cast<uint32_t>(m_depthRenderTargets.size());
    Clear();
    Add(device, numDepthRenderTargets, renderTargetWidth, renderTargetHeight);
}

void PostProcessDepthSinkBufferResource::Clear()
{
    m_depthRenderTargets.clear();
    m_depthRenderTargets.shrink_to_fit();
    m_depthRenderTargetViewsRT.clear();
    m_depthRenderTargetViewsRT.shrink_to_fit();
    m_depthRenderTargetViewsSR.clear();
    m_depthRenderTargetViewsSR.shrink_to_fit();
}

void PostProcessDepthSinkBufferResource::SetArrayRegion(const std::vector<Diligent::IDeviceObject*>& views, size_t offset, size_t count)
{
    m_variable->SetArray(
        views.data() + offset,
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
