#include "PassUtilities.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/pass/PostProcessPass.h"

#include "ncengine/graphics/GraphicsUtility.h"

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
auto MakePostProcessPassInstances(nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>
{
    const auto hasProperties = nc::PassHasProperties(passId);
    auto instances = std::vector<nc::graphics::PostProcessPipelineInstance>{};
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        if (!(passId & nc::GetCombinedPostProcessEffectPassFlags(effectId)))
        {
            continue;
        }

        instances.emplace_back(
            hasProperties
                ? std::optional{MakeDefaultPassProperties(passId)}
                : std::nullopt,
            effectId,
            false
        );
    }

    return instances;
}

void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       nc::graphics::PostProcessColorSinkBufferResource& postProcessColorSinkBufferResource,
                       nc::graphics::PostProcessDepthSinkBufferResource& postProcessDepthSinkBufferResource,
                       uint32_t colorRenderTargetIndex,
                       uint32_t depthRenderTargetIndex,
                       bool isMsaa)
{
    Diligent::ITextureView* pRTV = nullptr;
    Diligent::ITextureView* pDSV = nullptr;

    if (colorRenderTargetIndex != NoTarget)
    {
        if (colorRenderTargetIndex == SwapChainColorRTIndex)
        {
            pRTV = swapChain.GetCurrentBackBufferRTV();
        }
        else
        {
            pRTV = isMsaa ? static_cast<Diligent::ITextureView*>(postProcessColorSinkBufferResource.GetMsaaSrv(colorRenderTargetIndex)) :
                            static_cast<Diligent::ITextureView*>(postProcessColorSinkBufferResource.GetSrv(colorRenderTargetIndex));
        }
    }

    if (depthRenderTargetIndex != NoTarget)
    {
        if (depthRenderTargetIndex == SwapChainColorRTIndex)
        {
            pDSV = swapChain.GetDepthBufferDSV();
        }
        else
        {
            pDSV = isMsaa ? static_cast<Diligent::ITextureView*>(postProcessDepthSinkBufferResource.GetMsaaDSV(depthRenderTargetIndex)) :
                            static_cast<Diligent::ITextureView*>(postProcessDepthSinkBufferResource.GetDSV(depthRenderTargetIndex));
        }
    }

    constexpr auto ClearColor = nc::Vector4{0.050f, 0.050f, 0.050f, 1.0f};
    if (colorRenderTargetIndex != NoTarget)
    {
        context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    if (depthRenderTargetIndex != NoTarget)
    {
        context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
}


void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessColorSinkBufferResource& postProcessColorSinkBufferResource,
                      nc::graphics::PostProcessDepthSinkBufferResource& postProcessDepthSinkBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex,
                      bool isMsaa)
{
    Diligent::ITextureView* pRTV = nullptr;
    Diligent::ITextureView* pDSV = nullptr;

    if (colorRenderTargetIndex != NoTarget)
    {
        if (colorRenderTargetIndex == SwapChainColorRTIndex)
        {
            pRTV = swapChain.GetCurrentBackBufferRTV();
        }
        else
        {
            pRTV = isMsaa ? static_cast<Diligent::ITextureView*>(postProcessColorSinkBufferResource.GetMsaaSrv(colorRenderTargetIndex)) :
                            static_cast<Diligent::ITextureView*>(postProcessColorSinkBufferResource.GetSrv(colorRenderTargetIndex));
        }
    }

    if (depthRenderTargetIndex != NoTarget)
    {
        if (depthRenderTargetIndex == SwapChainColorRTIndex)
        {
            pDSV = swapChain.GetDepthBufferDSV();
        }
        else
        {
            pDSV = isMsaa ? static_cast<Diligent::ITextureView*>(postProcessDepthSinkBufferResource.GetMsaaDSV(depthRenderTargetIndex)) :
                            static_cast<Diligent::ITextureView*>(postProcessDepthSinkBufferResource.GetDSV(depthRenderTargetIndex));
        }
    }

    context.SetRenderTargets(colorRenderTargetIndex == NoTarget ? 0 : 1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

auto CreateShaderFromSourceIfInitialized(ShaderFactory& shaderFactory, Diligent::SHADER_TYPE shaderType, const nc::graphics::ShaderPaths& shaderPaths) -> Diligent::RefCntAutoPtr<Diligent::IShader>
{
    Diligent::RefCntAutoPtr<Diligent::IShader> shader;
    const auto& shaderPath = shaderType == Diligent::SHADER_TYPE_VERTEX ? shaderPaths.vertexShaderPath : shaderPaths.pixelShaderPath;

    if (shaderPath == "Uninitialized")
    {
        return shader;
    }

    auto source = shaderFactory.ReadShaderFile(shaderPath);
    shader = shaderFactory.MakeShaderFromSource(
        source,
        shaderPath.data(),
        shaderType,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    return shader;
}

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool
{
    return (colorRenderTargetIndex != SwapChainColorRTIndex || depthRenderTargetIndex != SwapChainDepthRTIndex);
}

auto ToPassBaseId(const ShaderPaths& shaderPaths, std::string_view name) -> size_t
{
    std::size_t hashCode = 0u;
    hashCode = HashCombine(hashCode, shaderPaths.pixelShaderPath);
    hashCode = HashCombine(hashCode, name);
    return HashCombine(hashCode, shaderPaths.vertexShaderPath);
}

auto NoTargets() -> std::vector<uint32_t> { return std::vector<uint32_t>{}; }
auto SingleSource(uint32_t target) -> std::vector<uint32_t> { return std::vector<uint32_t>{target}; }
} // namespace nc::graphics
