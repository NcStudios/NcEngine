#include "PassUtilities.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/pass/PostProcessPass.h"

#include "ncengine/graphics/GraphicsUtility.h"

#include <algorithm>
#include <array>
#include <span>

namespace
{
auto HashCombine(std::size_t hashCode, std::string_view inputString) -> std::size_t
{
    std::hash<std::string_view> hasher;
    hashCode ^= hasher(inputString) + 0x9e3779b9 + (hashCode << 6) + (hashCode >> 2); 
    return hashCode;
}
} // anonymous namespace

namespace nc::graphics
{
void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       PerPassResourceSignature& perPassResourceSignature,
                       uint32_t colorRenderTargetIndex,
                       uint32_t depthRenderTargetIndex,
                       bool isMsaa)
{
    Diligent::ITextureView* pRTV = ToColorRenderTargetView(swapChain, perPassResourceSignature.GetColorSinksResource(), colorRenderTargetIndex, isMsaa);
    Diligent::ITextureView* pDSV = ToDepthRenderTargetView(swapChain, perPassResourceSignature.GetDepthSinksResource(), depthRenderTargetIndex, isMsaa);

    if (pRTV)
    {
        context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    if (pDSV)
    {
        context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}

void ClearPostProcessRenderTarget(Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  SinkBufferResource& postProcessSinkBufferResource,
                                  uint32_t postProcessRenderTargetIndex)
{
    Diligent::ITextureView* pRTV = ToPostProcessRenderTargetView(swapChain, postProcessSinkBufferResource, postProcessRenderTargetIndex);

    if (pRTV)
    {
        context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}

void ClearPointShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                     CubeSinkBufferResource& shadowMapSinkBufferResource,
                                     uint32_t lightIndex,
                                     uint32_t faceIndex)
{
    Diligent::ITextureView* pDSV = shadowMapSinkBufferResource.GetDepthTargetView((lightIndex * 6) + faceIndex);
    if (pDSV)
    {
        context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}

void ClearUniShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                   SinkBufferResource& shadowMapSinkBufferResource,
                                   uint32_t shadowMapRenderTargetIndex)
{
    Diligent::ITextureView* pDSV = shadowMapSinkBufferResource.GetRenderTargetView(shadowMapRenderTargetIndex);
    if (pDSV)
    {
        context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      PerPassResourceSignature& perPassResourceSignature,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex,
                      bool isMsaa)
{
    Diligent::ITextureView* pRTV = ToColorRenderTargetView(swapChain, perPassResourceSignature.GetColorSinksResource(), colorRenderTargetIndex, isMsaa);
    Diligent::ITextureView* pDSV = ToDepthRenderTargetView(swapChain, perPassResourceSignature.GetDepthSinksResource(), depthRenderTargetIndex, isMsaa);
    context.SetRenderTargets(pRTV ? 1 : 0, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void BindPostProcessRenderTarget(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 SinkBufferResource& postProcessSinkBufferResource,
                                 uint32_t postProcessRenderTargetIndex)
{
    Diligent::ITextureView* pRTV = ToPostProcessRenderTargetView(swapChain, postProcessSinkBufferResource, postProcessRenderTargetIndex);
    context.SetRenderTargets(pRTV ? 1 : 0, &pRTV, nullptr, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void BindPointShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                    CubeSinkBufferResource& shadowMapSinkBufferResource,
                                    uint32_t lightIndex,
                                    uint32_t faceIndex)
{
    Diligent::ITextureView* pDSV = shadowMapSinkBufferResource.GetDepthTargetView((lightIndex *6)+ faceIndex);
    context.SetRenderTargets(0, nullptr, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void BindUniShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                  SinkBufferResource& shadowMapSinkBufferResource,
                                  uint32_t shadowMapRenderTargetIndex)
{
    Diligent::ITextureView* pDSV = shadowMapSinkBufferResource.GetRenderTargetView(shadowMapRenderTargetIndex);
    context.SetRenderTargets(0, nullptr, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

auto GetSinks(const PassManifest& passManifest, const PassDesc& passDesc) -> Sinks
{
    return Sinks
    {
        .color = passManifest.GetColorTargetIndex(passDesc.colorSink),
        .depth = passManifest.GetDepthTargetIndex(passDesc.depthSink),
        .postProcess = passManifest.GetPostProcessTargetIndex(passDesc.postProcessSink)
    };
}

auto HasAnyColorSources(const Sources& sources) -> bool
{
    return std::ranges::any_of(sources.color, [](auto source)
    {
        return source != SwapChainTarget && source != NoTarget;
    });
}

auto HasAnyDepthSources(const Sources& sources) -> bool
{
    return std::ranges::any_of(sources.depth, [](auto source)
    {
        return source != DepthStencilTarget && source != NoTarget;
    });
}

auto GetSources(const PassManifest& passManifest, const PassDesc& passDesc) -> Sources
{
    auto sources = Sources{};

    std::ranges::transform(passDesc.colorSources, std::back_inserter(sources.color),
        [&passManifest](const auto& colorBuffer) { return passManifest.GetColorTargetIndex(colorBuffer); });

    std::ranges::transform(passDesc.depthSources, std::back_inserter(sources.depth),
        [&passManifest](const auto& depthBuffer) { return passManifest.GetDepthTargetIndex(depthBuffer); });

    sources.postProcess = passManifest.GetPostProcessTargetIndex(passDesc.postProcessSource);

    return sources;
}

auto ToPassBaseId(const ShaderPaths& shaderPaths, std::string_view name) -> size_t
{
    std::size_t hashCode = 0u;
    hashCode = HashCombine(hashCode, shaderPaths.pixelShaderPath);
    hashCode = HashCombine(hashCode, name);
    return HashCombine(hashCode, shaderPaths.vertexShaderPath);
}

auto ToColorRenderTargetView(Diligent::ISwapChain& swapChain, SinkBufferResource& colorSinkBufferResource, uint32_t index, bool isMsaa) -> Diligent::ITextureView*
{
    if (index == SwapChainTarget)
    {
        return swapChain.GetCurrentBackBufferRTV();
    }

    if (index == NoTarget)
    {
        return nullptr;
    }

    return isMsaa ? colorSinkBufferResource.GetMsaaRenderTargetView(index) : colorSinkBufferResource.GetRenderTargetView(index);
}

auto ToDepthRenderTargetView(Diligent::ISwapChain& swapChain, SinkBufferResource& depthSinkBufferResource, uint32_t index, bool isMsaa) -> Diligent::ITextureView*
{
    if (index == DepthStencilTarget)
    {
        return swapChain.GetDepthBufferDSV();
    }

    if (index == NoTarget)
    {
        return nullptr;
    }

    return isMsaa ? depthSinkBufferResource.GetMsaaRenderTargetView(index) : depthSinkBufferResource.GetRenderTargetView(index);
}

auto ToPostProcessRenderTargetView(Diligent::ISwapChain& swapChain, SinkBufferResource& postProcessSinkBufferResource, uint32_t postProcessRenderTargetIndex) -> Diligent::ITextureView*
{
    if (postProcessRenderTargetIndex == SwapChainTarget)
    {
        return swapChain.GetCurrentBackBufferRTV();
    }

    if (postProcessRenderTargetIndex == NoTarget)
    {
        return nullptr;
    }

    return postProcessSinkBufferResource.GetRenderTargetView(0);
}

auto ToPostProcessCubeRenderTargetView(Diligent::ISwapChain& swapChain, CubeSinkBufferResource& postProcessSinkBufferResource, uint32_t postProcessRenderTargetIndex) -> Diligent::ITextureView*
{
    if (postProcessRenderTargetIndex == SwapChainTarget)
    {
        return swapChain.GetCurrentBackBufferRTV();
    }

    if (postProcessRenderTargetIndex == NoTarget)
    {
        return nullptr;
    }

    return postProcessSinkBufferResource.GetRenderTargetView(0);
}

auto SingleSource(uint32_t target) -> std::vector<uint32_t> { return std::vector<uint32_t>{target}; }
} // namespace nc::graphics
