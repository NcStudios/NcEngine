#include "PassUtilities.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/pass/PostProcessPass.h"

#include "ncengine/graphics/GraphicsUtility.h"

#include <array>
#include <span>

namespace nc::graphics
{
auto MakeSwapChainPipelineCreateInfo(Diligent::IShader& vertexShader,
                                     Diligent::IShader& pixelShader,
                                     Diligent::ISwapChain& swapChain,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo
{
    using namespace Diligent;

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pVS = &vertexShader;
    ci.pPS = &pixelShader;

    ci.GraphicsPipeline.NumRenderTargets             = 1;
    ci.GraphicsPipeline.RTVFormats[0]                = swapChain.GetDesc().ColorBufferFormat;
    ci.GraphicsPipeline.DSVFormat                    = swapChain.GetDesc().DepthBufferFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return ci;
}

auto MakeOffScreenPipelineCreateInfo(Diligent::IShader& vertexShader,
                                     Diligent::IShader& pixelShader,
                                     Diligent::ISwapChain& swapChain,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo
{
    using namespace Diligent;

    auto ci = MakeSwapChainPipelineCreateInfo(vertexShader, pixelShader, swapChain, signatures, layoutElements, name);
    ci.GraphicsPipeline.RTVFormats[0] = OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat     = OffScreenDepthRTFormat;
    return ci;
}

auto MakeOffScreenPostProcessPipelineCreateInfo(Diligent::IShader& vertexShader,
                                                Diligent::IShader& pixelShader,
                                                Diligent::ISwapChain& swapChain,
                                                std::span<Diligent::IPipelineResourceSignature*> signatures,
                                                std::span<const Diligent::LayoutElement> layoutElements,
                                                std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo
{
    using namespace Diligent;

    auto ci = MakeSwapChainPipelineCreateInfo(vertexShader, pixelShader, swapChain, signatures, layoutElements, name);
    ci.GraphicsPipeline.RTVFormats[0] = OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat     = OffScreenDepthRTFormat;
    ci.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    return ci;
}

auto MakeSwapChainPostProcessPipelineCreateInfo(Diligent::IShader& vertexShader,
                                                Diligent::IShader& pixelShader,
                                                Diligent::ISwapChain& swapChain,
                                                std::span<Diligent::IPipelineResourceSignature*> signatures,
                                                std::span<const Diligent::LayoutElement> layoutElements,
                                                std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo
{
    using namespace Diligent;

    auto ci = MakeSwapChainPipelineCreateInfo(vertexShader, pixelShader, swapChain, signatures, layoutElements, name);
    ci.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    return ci;
}

auto MakePostProcessPropertyBuffer(Diligent::IDeviceContext& context,
                                   Diligent::IRenderDevice& device,
                                   nc::PostProcessPassFlag::type passId) -> nc::graphics::DynamicUniformBuffer
{
    switch (passId)
    {
        case nc::PostProcessPassFlag::Outline:
        {
            return nc::graphics::DynamicUniformBuffer(
                context,
                device,
                nc::graphics::OutlinePassData{},
                "OutlineDataBuffer"
            );
        }
    }

    throw nc::NcError(fmt::format("Unexpected post process pass '{}'", passId));
}

auto MakePostProcessPassInstances(Diligent::IDeviceContext& context,
                       Diligent::IRenderDevice& device,
                       nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>
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
                ? std::optional{MakePostProcessPropertyBuffer(context, device, passId)}
                : std::nullopt,
            effectId,
            false
        );
    }

    return instances;
}

auto MakeOffScreenMaterialPass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               std::span<Diligent::IPipelineResourceSignature*> signatures,
                               PostProcessSinkBufferResource& postProcessSinkBufferResource,
                               std::string_view pixelShaderPath,
                               std::string_view vertexShaderPath,
                               std::string_view pipelineName) -> MaterialPass
{
    auto vertexShader = shaderFactory.MakeShaderFromPath(
        vertexShaderPath,
        vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromPath(
        pixelShaderPath,
        pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeOffScreenPipelineCreateInfo(
        *vertexShader,
        *pixelShader,
        swapChain,
        signatures,
        layoutElements,
        pipelineName.data()
    );

    auto renderTargetIndices = postProcessSinkBufferResource.Add(device, 1, 1, swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    return MaterialPass(device, createInfo, MaterialPassFlag::Toon, renderTargetIndices[0], renderTargetIndices[1]);
}

auto MakeOffScreenPostProcessPass(Diligent::IRenderDevice& device,
                                  Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  ShaderFactory& shaderFactory,
                                  std::span<Diligent::IPipelineResourceSignature*> signatures,
                                  PostProcessSinkBufferResource& postProcessSinkBufferResource,
                                  PostProcessPassFlag::type passId,
                                  std::string_view pixelShaderPath,
                                  std::string_view vertexShaderPath,
                                  std::string_view pipelineName) -> PostProcessPipeline
{
    auto vertexShader = shaderFactory.MakeShaderFromPath(
        vertexShaderPath,
        vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromPath(
        pixelShaderPath,
        pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeOffScreenPostProcessPipelineCreateInfo(
        *vertexShader,
        *pixelShader,
        swapChain,
        signatures,
        layoutElements,
        pipelineName.data()
    );

    auto renderTargetIndices = postProcessSinkBufferResource.Add(device, 1, 1, swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    return PostProcessPipeline(device, createInfo, MakePostProcessPassInstances(context, device, passId), passId, renderTargetIndices[0], renderTargetIndices[1], static_cast<uint32_t>(renderTargetIndices.size()));
}

auto MakeSwapChainPostProcessPass(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                Diligent::ISwapChain& swapChain,
                                ShaderFactory& shaderFactory,
                                std::span<Diligent::IPipelineResourceSignature*> signatures,
                                PostProcessPassFlag::type passId,
                                std::string_view pixelShaderPath,
                                std::string_view vertexShaderPath,
                                std::string_view pipelineName) -> PostProcessPipeline
{
    auto vertexShader = shaderFactory.MakeShaderFromPath(
        vertexShaderPath,
        vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromPath(
        pixelShaderPath,
        pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeSwapChainPostProcessPipelineCreateInfo(
        *vertexShader,
        *pixelShader,
        swapChain,
        signatures,
        layoutElements,
        pipelineName.data()
    );

    return PostProcessPipeline(device, createInfo, MakePostProcessPassInstances(context, device, passId), passId, SwapChainColorRTIndex, SwapChainDepthRTIndex, 1);
}

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessSinkBufferResource& postProcessSinkBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex)
{
    Diligent::ITextureView* pRTV = nullptr;
    Diligent::ITextureView* pDSV = nullptr;

    if (colorRenderTargetIndex == SwapChainColorRTIndex)
    {
        pRTV = swapChain.GetCurrentBackBufferRTV();
    }
    else
    {
        pRTV = static_cast<Diligent::ITextureView*>(postProcessSinkBufferResource.GetColorRenderTarget(colorRenderTargetIndex));
    }

    if (depthRenderTargetIndex == SwapChainColorRTIndex)
    {
        pDSV = swapChain.GetDepthBufferDSV();
    }
    else
    {
        pDSV = static_cast<Diligent::ITextureView*>(postProcessSinkBufferResource.GetDepthRenderTarget(depthRenderTargetIndex));
    }

    context.SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    constexpr auto ClearColor = nc::Vector4{0.050f, 0.050f, 0.050f, 1.0f};
    context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool
{
    return (colorRenderTargetIndex != SwapChainColorRTIndex || depthRenderTargetIndex != SwapChainDepthRTIndex);
}
} // namespace nc::graphics
