#include "WireframePass.h"
#include "PassTypes.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/SinkBufferResource.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"

#include "ncutility/NcError.h"

namespace
{
auto MakePso(Diligent::IRenderDevice& device,
             nc::graphics::ShaderFactory& shaderFactory,
             Diligent::IPipelineResourceSignature& perFrameResourceSignature,
             const nc::graphics::PassDesc& passDesc,
             uint32_t numSamples)
{
    using namespace Diligent;

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

    auto signatures = std::array{&perFrameResourceSignature};
    auto layoutElements = nc::graphics::GetMeshVertexLayoutElements(0);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pVS = vertexShader;
    ci.pPS = pixelShader;

    ci.GraphicsPipeline.NumRenderTargets             = passDesc.colorSink == nc::graphics::NoTarget ? 0 : 1;
    ci.GraphicsPipeline.RTVFormats[0]                = nc::graphics::OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                    = passDesc.depthSink == nc::graphics::NoTarget ? TEX_FORMAT_UNKNOWN : nc::graphics::OffScreenDepthRTFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = passDesc.depthSink == nc::graphics::NoTarget ? False : True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.RasterizerDesc.FillMode      = FILL_MODE_WIREFRAME;
    ci.GraphicsPipeline.SmplDesc.Count               = static_cast<uint8_t>(numSamples);

    auto pso = RefCntAutoPtr<IPipelineState>{};
    device.CreateGraphicsPipelineState(ci, &pso);
    if (!pso)
    {
        throw nc::NcError{"Failed to create wireframe pipeline"};
    }

    return pso;
}
} // anonymous namespace

namespace nc::graphics
{
WireframePass::WireframePass(Diligent::IRenderDevice& device,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings,
                             const PassDesc& passDesc,
                             uint32_t numSamples)
    : pso{MakePso(device, shaderFactory, shaderBindings.GetPerFrameSignature().GetResourceSignature(), passDesc, numSamples)},
      buffer{&shaderBindings.GetPerFrameSignature().GetWireframeBuffer()},
      colorRTIndex{passDesc.colorSink},
      depthRTIndex{passDesc.depthSink}
{
}
} // namespace nc::graphics
