#include "SkyboxPass.h"
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
                    const PipelineShaders& shaders,
                    ShaderBindings& shaderBindings,
                    const PassDesc& passDesc) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    const auto layoutElements = GetMeshVertexLayoutElements(passDesc.layoutElements);

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = passDesc.name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.pPS = shaders.pixelShader;
    ci.pVS = shaders.vertexShader;

    auto colorFormat = OffScreenColorRTFormat;

    auto depthFormat = TEX_FORMAT_UNKNOWN;
    if (passDesc.depthSink != DepthTarget::None)
    {
        depthFormat = OffScreenDepthRTFormat;
    }

    ci.GraphicsPipeline.NumRenderTargets                  = 1;
    ci.GraphicsPipeline.RTVFormats[0]                     = colorFormat;
    ci.GraphicsPipeline.DSVFormat                         = depthFormat;
    ci.GraphicsPipeline.RasterizerDesc.CullMode           = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable      = passDesc.useDepthTest;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = false;
    ci.GraphicsPipeline.DepthStencilDesc.DepthFunc        = COMPARISON_FUNC_LESS_EQUAL; // Forces everything less than 1.0 (depth set in vertex shader) to render in front of the skybox
    ci.GraphicsPipeline.InputLayout.LayoutElements        = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements           = static_cast<uint32_t>(layoutElements.size());

    ci.GraphicsPipeline.SmplDesc.Count               = static_cast<uint8_t>(1);
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

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
SkyboxPass::SkyboxPass(Diligent::IRenderDevice& device,
                       const PipelineShaders& shaders,
                       ShaderBindings& shaderBindings,
                       const PassManifest& passManifest,
                       const PassDesc& passDesc)
    : Pass{CreatePipeline(device, shaders, shaderBindings, passDesc), GetSinks(passManifest, passDesc), GetSources(passManifest, passDesc)}
{
}

} // namespace nc::graphics
