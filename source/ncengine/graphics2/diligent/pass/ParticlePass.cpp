#include "ParticlePass.h"
#include "PassTypes.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/PostProcessColorSinkBufferResource.h"
#include "graphics2/diligent/resource/PostProcessDepthSinkBufferResource.h"
#include "graphics2/diligent/resource/PostProcessSinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncutility/NcError.h"

namespace
{
auto MakePso(Diligent::IRenderDevice& device,
             nc::graphics::ShaderFactory& shaderFactory,
             Diligent::IPipelineResourceSignature& perFrameResourceSignature,
             const nc::graphics::PassDesc& passDesc)
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
    NC_ASSERT(passDesc.depthSink != nc::graphics::NoTarget, "nope");
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    // ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = passDesc.depthSink == nc::graphics::NoTarget ? False : True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.RasterizerDesc.FillMode      = FILL_MODE_SOLID;

    auto& renderTarget = ci.GraphicsPipeline.BlendDesc.RenderTargets[0];
    renderTarget.BlendEnable = true;
    renderTarget.SrcBlend = BLEND_FACTOR_SRC_ALPHA;
    renderTarget.DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;
    renderTarget.BlendOp     = BLEND_OPERATION_ADD;
    renderTarget.SrcBlendAlpha = BLEND_FACTOR_ONE;
    renderTarget.DestBlendAlpha = BLEND_FACTOR_INV_SRC_ALPHA;
    // renderTarget.DestBlendAlpha = BLEND_FACTOR_ZERO;
    renderTarget.BlendOpAlpha   = BLEND_OPERATION_ADD;

    auto pso = RefCntAutoPtr<IPipelineState>{};
    device.CreateGraphicsPipelineState(ci, &pso);
    if (!pso)
    {
        throw nc::NcError{"Failed to create particle pipeline"};
    }

    return pso;
}
} // anonymous namespace

namespace nc::graphics
{
ParticlePass::ParticlePass(Diligent::IRenderDevice& device,
                           ShaderFactory& shaderFactory,
                           ShaderBindings& shaderBindings,
                           const PassDesc& passDesc)
    : pso{MakePso(device, shaderFactory, shaderBindings.GetPerFrameSignature().GetResourceSignature(), passDesc)},
      colorRTIndex{passDesc.colorSink},
      depthRTIndex{passDesc.depthSink}
{
}
} // namespace nc::graphics
