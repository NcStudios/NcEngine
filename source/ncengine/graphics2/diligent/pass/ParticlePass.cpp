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

    ci.GraphicsPipeline.NumRenderTargets                  = passDesc.colorSink == ColorTarget::None ? 0 : 1;
    ci.GraphicsPipeline.RTVFormats[0]                     = passDesc.colorSink == ColorTarget::None ? TEX_FORMAT_UNKNOWN : OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                         = passDesc.depthSink == DepthTarget::None ? TEX_FORMAT_UNKNOWN : OffScreenDepthRTFormat;
    ci.GraphicsPipeline.RasterizerDesc.CullMode           = ToDiligentCullMode(passDesc.cullMode);
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable      = passDesc.useDepthTest;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    ci.GraphicsPipeline.InputLayout.LayoutElements        = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements           = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.RasterizerDesc.FillMode           = FILL_MODE_SOLID;
    ci.GraphicsPipeline.SmplDesc.Count                    = passDesc.isMsaa ? static_cast<uint8_t>(numSamples) : static_cast<uint8_t>(1);
    ci.GraphicsPipeline.PrimitiveTopology                 = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    if (passDesc.alphaBlend)
    {
        auto& renderTarget          = ci.GraphicsPipeline.BlendDesc.RenderTargets[0];
        renderTarget.BlendEnable    = true;
        renderTarget.SrcBlend       = BLEND_FACTOR_SRC_ALPHA;
        renderTarget.DestBlend      = BLEND_FACTOR_INV_SRC_ALPHA;
        renderTarget.BlendOp        = BLEND_OPERATION_ADD;
        renderTarget.SrcBlendAlpha  = BLEND_FACTOR_ONE;
        renderTarget.DestBlendAlpha = BLEND_FACTOR_INV_SRC_ALPHA;
        renderTarget.BlendOpAlpha   = BLEND_OPERATION_ADD;
    }

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
ParticlePass::ParticlePass(Diligent::IRenderDevice& device,
                           const PipelineShaders& shaders,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest,
                           const PassDesc& passDesc,
                           uint32_t numSamples)
    : Pass{
        CreatePipeline(device, shaders, shaderBindings, passDesc, numSamples),
        GetSinks(passManifest, passDesc),
        GetSources(passManifest, passDesc)
      },
      isMsaa{numSamples > 1}
{
}
} // namespace nc::graphics
