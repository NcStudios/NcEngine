#include "WireframePass.h"
#include "PassTypes.h"
#include "PassUtilities.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/SinkBufferResource.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"

#include "ncutility/NcError.h"

namespace
{
using namespace Diligent;
using namespace nc::graphics;

auto CreatePipeline(Diligent::IRenderDevice& device,
                    Diligent::ISwapChain& swapChain,
                    const PipelineShaders& shaders,
                    ShaderBindings& shaderBindings,
                    const PassDesc& passDesc,
                    uint32_t numSamples) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    const auto layoutElements = GetMeshVertexLayoutElements(passDesc.layoutElements);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.pPS = shaders.pixelShader;
    ci.pVS = shaders.vertexShader;

    const auto textureFormat = ToTextureFormat(swapChain, 
                                            passDesc.colorSink,
                                            passDesc.depthSink,
                                            passDesc.shadowMapSink,
                                            passDesc.postProcessSink);

    ci.GraphicsPipeline.NumRenderTargets                  = passDesc.numRenderTargets;
    ci.GraphicsPipeline.RTVFormats[0]                     = textureFormat.colorFormat;
    ci.GraphicsPipeline.DSVFormat                         = textureFormat.depthFormat;
    ci.GraphicsPipeline.RasterizerDesc.CullMode           = ToDiligentCullMode(passDesc.cullMode);
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable      = passDesc.useDepthTest;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    ci.GraphicsPipeline.InputLayout.LayoutElements        = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements           = static_cast<uint32_t>(layoutElements.size());

    ci.GraphicsPipeline.RasterizerDesc.FillMode           = FILL_MODE_WIREFRAME;
    ci.GraphicsPipeline.SmplDesc.Count                    = passDesc.isMsaa.value ? static_cast<uint8_t>(numSamples) : static_cast<uint8_t>(1);
    ci.GraphicsPipeline.PrimitiveTopology                 = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature()};
    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    auto pso = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    device.CreateGraphicsPipelineState(ci, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object")

    return pso;
}
} // anonymous namespace

namespace nc::graphics
{
WireframePass::WireframePass(Diligent::IRenderDevice& device,
                             Diligent::ISwapChain& swapChain,
                             const PipelineShaders& shaders,
                             ShaderBindings& shaderBindings,
                             const PassManifest& passManifest,
                             const PassDesc& passDesc,
                             uint32_t numSamples)
    : Pass{
        CreatePipeline(device, swapChain, shaders, shaderBindings, passDesc, numSamples),
        GetSinks(passManifest, passDesc),
        GetSources(passManifest, passDesc)
      },
      buffer{&shaderBindings.GetPerFrameSignature().GetWireframeBuffer()},
      isMsaa{numSamples > 1}
{
}
} // namespace nc::graphics
