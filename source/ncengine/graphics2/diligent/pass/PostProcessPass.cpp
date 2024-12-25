#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/PostProcessSinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncengine/graphics/GraphicsUtility.h"

namespace nc::graphics
{
using namespace Diligent;

PostProcessPass::PostProcessPass(IRenderDevice& device,
                                 const GraphicsPipelineStateCreateInfo& createInfo,
                                 std::vector<PostProcessPipelineInstance> instances_,
                                 PassDesc passDesc_)
    : pso{},
      instances{std::move(instances_)},
      passDesc{passDesc_}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakePostProcessPass(IRenderDevice& device,
                         ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         PassDesc passDesc) -> PostProcessPass
{
    auto layoutElements = GetMeshVertexLayoutElements(0);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature(), &shaderBindings.GetPerPassSignature().GetResourceSignature()};
    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    RefCntAutoPtr<IShader> pixelShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_PIXEL, passDesc.shaderPaths);
    RefCntAutoPtr<IShader> vertexShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_VERTEX, passDesc.shaderPaths);
    ci.pPS = pixelShader;
    ci.pVS = vertexShader;

    if (passDesc.colorSink == SwapChainColorRTIndex)
    {
        ci.GraphicsPipeline.RTVFormats[0] = swapChain.GetDesc().ColorBufferFormat;
    }
    else if (passDesc.colorSink == NoTarget)
    {
        ci.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_UNKNOWN;
    }
    else
    {
        ci.GraphicsPipeline.RTVFormats[0] = OffScreenColorRTFormat;
    }

    if (passDesc.depthSink == SwapChainDepthRTIndex)
    {
        ci.GraphicsPipeline.DSVFormat = swapChain.GetDesc().DepthBufferFormat;
    }
    else if (passDesc.depthSink == NoTarget)
    {
        ci.GraphicsPipeline.DSVFormat = TEX_FORMAT_UNKNOWN;
    }
    else
    {
        ci.GraphicsPipeline.DSVFormat = OffScreenDepthRTFormat;
    }

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == NoTarget ? 0 : 1;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = passDesc.depthSink == NoTarget ? False : True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return PostProcessPass(device,
                           ci,
                           MakePostProcessPassInstances(passDesc.id),
                           passDesc);
}

auto MakePostProcessPasses(IRenderDevice& device,
                           ISwapChain& swapChain,
                           ShaderFactory& shaderFactory,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest) -> std::vector<PostProcessPass>
{
    auto passes = std::vector<PostProcessPass>{};
    const auto& passDescs = passManifest.PostProcessPassDescs();

    if (passDescs.empty())
        return passes;

    passes.reserve(passDescs.size());

    for (auto& passDesc : passDescs)
    {
        passes.emplace_back(MakePostProcessPass(device, swapChain, shaderFactory, shaderBindings, passDesc));
    }

    return passes;
}
} // namespace nc::graphics
