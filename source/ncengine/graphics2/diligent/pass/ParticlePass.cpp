#include "ParticlePass.h"
#include "PassTypes.h"
#include "PassUtilities.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncutility/NcError.h"

namespace
{
using namespace Diligent;
using namespace nc::graphics;

auto CreatePipeline(Diligent::IRenderDevice& device,
                    ShaderFactory& shaderFactory,
                    ShaderBindings& shaderBindings,
                    const PassDesc& passDesc,
                    uint32_t numSamples) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature()};
    auto layoutElements = GetMeshVertexLayoutElements(0);

    RefCntAutoPtr<IShader> pixelShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_PIXEL, passDesc.shaderPaths);
    RefCntAutoPtr<IShader> vertexShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_VERTEX, passDesc.shaderPaths);
    ci.pPS = pixelShader;
    ci.pVS = vertexShader;

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pVS = vertexShader;
    ci.pPS = pixelShader;

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == ColorBuffer::None ? 0 : 1;
    ci.GraphicsPipeline.RTVFormats[0]                = passDesc.colorSink == ColorBuffer::None ? TEX_FORMAT_UNKNOWN : OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                    = passDesc.depthSink == DepthBuffer::None ? TEX_FORMAT_UNKNOWN : OffScreenDepthRTFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.RasterizerDesc.FillMode      = FILL_MODE_SOLID;
    ci.GraphicsPipeline.SmplDesc.Count               = static_cast<uint8_t>(numSamples);

    auto& renderTarget = ci.GraphicsPipeline.BlendDesc.RenderTargets[0];
    renderTarget.BlendEnable = true;
    renderTarget.SrcBlend = BLEND_FACTOR_SRC_ALPHA;
    renderTarget.DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;
    renderTarget.BlendOp     = BLEND_OPERATION_ADD;
    renderTarget.SrcBlendAlpha = BLEND_FACTOR_ONE;
    renderTarget.DestBlendAlpha = BLEND_FACTOR_INV_SRC_ALPHA;
    renderTarget.BlendOpAlpha   = BLEND_OPERATION_ADD;

    auto pso = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    device.CreateGraphicsPipelineState(ci, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object")

    return pso;
}
} // anonymous namespace

namespace nc::graphics
{
ParticlePass::ParticlePass(Diligent::IRenderDevice& device,
                           ShaderFactory& shaderFactory,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest,
                           const PassDesc& passDesc,
                           uint32_t numSamples)
    : Pass(CreatePipeline(device, shaderFactory, shaderBindings, passDesc, numSamples), GetSinks(passManifest, passDesc), GetSources(passManifest, passDesc)),
      isMsaa{numSamples > 1}
{
}
} // namespace nc::graphics
