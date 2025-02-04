#include "MaterialPass.h"
#include "PassUtilities.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include <ranges>

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
    auto layoutElements = GetMeshVertexLayoutElements(0);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature()};
    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    RefCntAutoPtr<IShader> pixelShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_PIXEL, passDesc.shaderPaths);
    RefCntAutoPtr<IShader> vertexShader = CreateShaderFromSourceIfInitialized(shaderFactory, SHADER_TYPE_VERTEX, passDesc.shaderPaths);
    ci.pPS = pixelShader;
    ci.pVS = vertexShader;

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == ColorBuffer::None ? 0 : 1;
    ci.GraphicsPipeline.RTVFormats[0]                = passDesc.colorSink == ColorBuffer::None ? TEX_FORMAT_UNKNOWN : OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                    = passDesc.depthSink == DepthBuffer::None ? TEX_FORMAT_UNKNOWN : OffScreenDepthRTFormat;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.SmplDesc.Count               = passDesc.isMsaa ? static_cast<uint8_t>(numSamples) : static_cast<uint8_t>(1);

    auto pso = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    device.CreateGraphicsPipelineState(ci, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object")

    return pso;
}
} // anonymous namespace

namespace nc::graphics
{
MaterialPass::MaterialPass(Diligent::IRenderDevice& device,
                           ShaderFactory& shaderFactory,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest,
                           const PassDesc& passDesc,
                           uint32_t numSamples)
    : Pass{CreatePipeline(device, shaderFactory, shaderBindings, passDesc, numSamples), GetSinks(passManifest, passDesc), GetSources(passManifest, passDesc)},
      id{passDesc.id},
      isMsaa{passDesc.isMsaa}
{
}

} // namespace nc::graphics
