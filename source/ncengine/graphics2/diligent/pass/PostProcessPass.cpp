#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/PostProcessSinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncengine/graphics/GraphicsUtility.h"

namespace nc::graphics
{
PostProcessPass::PostProcessPass(Diligent::IRenderDevice& device,
                                 const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                                 std::vector<PostProcessPipelineInstance> instances_,
                                 PassDesc passDesc)
    : pso{},
      instances{std::move(instances_)},
      passDesc{passDesc}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakePostProcessPass(Diligent::IRenderDevice& device,
                         Diligent::ISwapChain& swapChain,
                         ShaderFactory& shaderFactory,
                         ShaderBindings& shaderBindings,
                         PassDesc passDesc) -> PostProcessPass
{
    auto pixelShader = shaderFactory.MakeShaderFromPath(
        passDesc.shaderPaths.pixelShaderPath,
        passDesc.shaderPaths.pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto vertexShader = shaderFactory.MakeShaderFromPath(
        passDesc.shaderPaths.vertexShaderPath,
        passDesc.shaderPaths.vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto layoutElements = GetMeshVertexLayoutElements(0);

    auto ci = Diligent::GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature(), &shaderBindings.GetPerPassSignature().GetResourceSignature()};

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pPS = pixelShader;
    ci.pVS = vertexShader;

    if (passDesc.colorSink == SwapChainColorRTIndex)
    {
        ci.GraphicsPipeline.RTVFormats[0] = swapChain.GetDesc().ColorBufferFormat;
    }
    else if (passDesc.colorSink == NoTarget)
    {
        ci.GraphicsPipeline.RTVFormats[0] = Diligent::TEX_FORMAT_UNKNOWN;
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
        ci.GraphicsPipeline.DSVFormat = Diligent::TEX_FORMAT_UNKNOWN;
    }
    else
    {
        ci.GraphicsPipeline.DSVFormat = OffScreenDepthRTFormat;
    }

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == NoTarget ? 0 : 1;
    ci.GraphicsPipeline.PrimitiveTopology            = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = Diligent::CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = passDesc.depthSink == NoTarget ? Diligent::False : Diligent::True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return PostProcessPass(device,
                           ci,
                           MakePostProcessPassInstances(passDesc.id),
                           passDesc);
}

auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
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
