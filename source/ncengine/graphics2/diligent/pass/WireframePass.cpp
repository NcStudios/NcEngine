#include "WireframePass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/ShaderBindings.h"
#include "graphics2/diligent/resource/WireframeBufferResource.h"

#include "ncutility/NcError.h"

namespace
{
auto MakePso(Diligent::IRenderDevice& device,
             Diligent::ISwapChain& swapChain,
             nc::graphics::ShaderFactory& shaderFactory,
             Diligent::IPipelineResourceSignature& perFrameResourceSignature)
{
    using namespace Diligent;

    std::string_view pixelShaderPath = "Wireframe.psh";
    std::string_view vertexShaderPath = "Wireframe.vsh";

    auto pixelShader = shaderFactory.MakeShaderFromPath(
        pixelShaderPath,
        pixelShaderPath.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto vertexShader = shaderFactory.MakeShaderFromPath(
        vertexShaderPath,
        vertexShaderPath.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto signatures = std::array{&perFrameResourceSignature};
    auto layoutElements = nc::graphics::GetMeshVertexLayoutElements(0);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = "WireframePipeline";
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pVS = vertexShader;
    ci.pPS = pixelShader;

    ci.GraphicsPipeline.NumRenderTargets             = 1;
    ci.GraphicsPipeline.RTVFormats[0]                = swapChain.GetDesc().ColorBufferFormat;
    ci.GraphicsPipeline.DSVFormat                    = swapChain.GetDesc().DepthBufferFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());
    ci.GraphicsPipeline.RasterizerDesc.FillMode      = FILL_MODE_WIREFRAME;

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
                             Diligent::ISwapChain& swapChain,
                             ShaderFactory& shaderFactory,
                             ShaderBindings& shaderBindings)
    : m_pso{MakePso(device, swapChain, shaderFactory, shaderBindings.GetPerFrameSignature().GetResourceSignature())},
      m_buffer{&shaderBindings.GetPerFrameSignature().GetWireframeBuffer()}
{
}

void WireframePass::Render(Diligent::IDeviceContext& context,
                           const WireframeRendererRenderState& state)
{
    if (state.wireframeData.empty())
    {
        return;
    }

    context.SetPipelineState(m_pso);
    for (const auto& [data, mesh] : state.wireframeData)
    {
        m_buffer->Update(context, data);
        const auto attribs = Diligent::DrawIndexedAttribs{
            mesh.indexCount,
            Diligent::VT_UINT32,
            Diligent::DRAW_FLAG_VERIFY_ALL,
            1,
            mesh.firstIndex,
            mesh.firstVertex,
            0
        };

        context.DrawIndexed(attribs);
    }
}
} // namespace nc::graphics
