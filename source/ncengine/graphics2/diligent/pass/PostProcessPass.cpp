#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/pass/PassUtilities.h"
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
                         Diligent::IDeviceContext& context,
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

    if (passDesc.sinks.first == SwapChainColorRTIndex || passDesc.sinks.second == SwapChainDepthRTIndex)
    {
        ci.GraphicsPipeline.RTVFormats[0] = swapChain.GetDesc().ColorBufferFormat;
        ci.GraphicsPipeline.DSVFormat = swapChain.GetDesc().DepthBufferFormat;
    }
    else
    {
        ci.GraphicsPipeline.RTVFormats[0] = OffScreenColorRTFormat;
        ci.GraphicsPipeline.DSVFormat = OffScreenDepthRTFormat;
    }

    ci.GraphicsPipeline.NumRenderTargets             = 1;
    ci.GraphicsPipeline.PrimitiveTopology            = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = Diligent::CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return PostProcessPass(device,
                           ci,
                           MakePostProcessPassInstances(context, device, passDesc.id),
                           passDesc);
}

auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::IDeviceContext& context,
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
        passes.emplace_back(MakePostProcessPass(device, context, swapChain, shaderFactory, shaderBindings, passDesc));
    }

    return passes;
}
} // namespace nc::graphics
