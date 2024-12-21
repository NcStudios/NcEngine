#include "MaterialPass.h"
#include "PassUtilities.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/PostProcessSinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include <ranges>

namespace
{
using namespace Diligent;
using namespace nc::graphics;

auto MakeMaterialPass(Diligent::IRenderDevice& device,
                      ShaderFactory& shaderFactory,
                      ShaderBindings& shaderBindings,
                      const PassDesc& passDesc,
                      uint32_t numSamples) -> MaterialPass
{
    auto pixelShaderSource = shaderFactory.ReadShaderFile(passDesc.shaderPaths.pixelShaderPath);
    auto pixelShader = shaderFactory.MakeShaderFromSource(
        pixelShaderSource,
        passDesc.shaderPaths.pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto vertexShaderSource = shaderFactory.ReadShaderFile(passDesc.shaderPaths.vertexShaderPath);
    auto vertexShader = shaderFactory.MakeShaderFromSource(
        vertexShaderSource,
        passDesc.shaderPaths.vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto layoutElements = GetMeshVertexLayoutElements(0);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature()};

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pPS = pixelShader;
    ci.pVS = vertexShader;

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == NoTarget ? 0 : 1;
    ci.GraphicsPipeline.RTVFormats[0]                = OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                    = passDesc.depthSink == NoTarget ? TEX_FORMAT_UNKNOWN : OffScreenDepthRTFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = passDesc.depthSink == NoTarget ? False : True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.SmplDesc.Count               = static_cast<uint8_t>(numSamples);

    return MaterialPass(device, ci, passDesc.id, passDesc.colorSink, passDesc.depthSink);
}
} // anonymous namespace

namespace nc::graphics
{
MaterialPass::MaterialPass(Diligent::IRenderDevice& device,
                           const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                           MaterialPassFlag::type passId,
                           uint32_t colorRTIndex_,
                           uint32_t depthRTIndex_)
    : pso{},
      id{passId},
      colorRTIndex{colorRTIndex_},
      depthRTIndex{depthRTIndex_}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakeMaterialPasses(Diligent::IRenderDevice& device,
                        ShaderFactory& shaderFactory,
                        ShaderBindings& shaderBindings,
                        std::span<const PassDesc> passManifest,
                        uint32_t numSamples) -> std::vector<MaterialPass>
{
    auto materialPasses = std::vector<MaterialPass>{};
    materialPasses.reserve(passManifest.size());

    for (auto& passDesc : passManifest)
    {
        materialPasses.emplace_back(MakeMaterialPass(device, shaderFactory, shaderBindings, passDesc, numSamples));
    }

    return materialPasses;
}
} // namespace nc::graphics
