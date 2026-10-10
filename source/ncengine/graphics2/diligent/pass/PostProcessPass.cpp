#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncengine/graphics/GraphicsUtility.h"

using namespace Diligent;
using namespace nc;
using namespace nc::graphics;

namespace
{
auto MakePostProcessPassInstances(PostProcessPassFlag::type passId) -> std::vector<PostProcessPipelineInstance>
{
    const auto hasProperties = PassHasProperties(passId);
    auto instances = std::vector<PostProcessPipelineInstance>{};
    for (const auto effectId : GetPostProcessEffectIds())
    {
        if (!(passId & GetCombinedPostProcessEffectPassFlags(effectId)))
        {
            continue;
        }

        instances.emplace_back(
            hasProperties
                ? std::optional{MakeDefaultPassProperties(passId)}
                : std::nullopt,
            effectId,
            false
        );
    }

    return instances;
}

auto CreatePipeline(Diligent::IRenderDevice& device,
                    ISwapChain& swapChain,
                    const PipelineShaders& shaders,
                    ShaderBindings& shaderBindings,
                    const PassDesc& passDesc) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
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
    ci.GraphicsPipeline.PrimitiveTopology                 = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    ci.GraphicsPipeline.RasterizerDesc.CullMode           = ToDiligentCullMode(passDesc.cullMode);
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable      = passDesc.useDepthTest;
    ci.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = passDesc.depthSink != DepthTarget::None;
    ci.GraphicsPipeline.InputLayout.LayoutElements        = nullptr;
    ci.GraphicsPipeline.InputLayout.NumElements           = 0;

    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature(), &shaderBindings.GetPerPassSignature().GetResourceSignature()};
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

PostProcessPass::PostProcessPass(IRenderDevice& device,
                                 ISwapChain& swapChain,
                                 const PipelineShaders& shaders,
                                 ShaderBindings& shaderBindings,
                                 const PassManifest& passManifest,
                                 const PassDesc& passDesc,
                                 bool isFinalPass)
    : Pass{
        CreatePipeline(device, swapChain, shaders, shaderBindings, passDesc),
        GetSinks(passManifest, passDesc),
        GetSources(passManifest, passDesc)
        },
      instances{isFinalPass ? std::vector<PostProcessPipelineInstance>{} : MakePostProcessPassInstances(passDesc.flag)},
      name{passDesc.name},
      flag{passDesc.flag}
{
}
} // namespace nc::graphics
