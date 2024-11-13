#include "Pass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/GlobalMeshBuffer.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

namespace
{
constexpr auto g_pixelShader = std::string_view{
R"(#ifdef VULKAN
// NonUniformResourceIndex is not supported by GLSLang
#   define NonUniformResourceIndex(x) x
#endif

Texture2D     Textures[];
SamplerState  Textures_sampler; // By convention, texture samplers must use the '_sampler' suffix

struct MaterialData
{
    float3 gradientStart;
    uint diffuseTexture;
    float3 gradientEnd;
    uint normalIndex;
    float3 outlineColor;
    float outlineWidth;
};

StructuredBuffer<MaterialData> MaterialDataBuffer : register(t1);

struct PSInput 
{ 
    float4 Pos           : SV_POSITION; 
    float2 UV            : TEX_COORD; 
    uint   MaterialIndex;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    float4 Color;
    uint TexIndex = MaterialDataBuffer[PSIn.MaterialIndex].diffuseTexture;
    Color = Textures[TexIndex].Sample(Textures_sampler, PSIn.UV);
    PSOut.Color = Color;
})"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    // Vertex attributes
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
    float3 Tangent     : ATTRIB3;
    float3 Bitangent   : ATTRIB4;
    float4 BoneWeights : ATTRIB5;
    uint4  BoneIds     : ATTRIB6;
};

struct PSInput 
{
    float4 Pos           : SV_POSITION;
    float2 UV            : TEX_COORD;
    uint   MaterialIndex;
};

struct MeshRendererData
{
    float4x4 model;
    uint materialIndex;
};

StructuredBuffer<MeshRendererData> MeshRendererBufferData;

cbuffer EnvironmentData
{
    float4x4 cameraViewProjection;
};

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), MeshRendererBufferData[InstanceID].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.MaterialIndex = MeshRendererBufferData[InstanceID].materialIndex;
}
)"};
} // anonymous namespace

namespace nc::graphics
{
Pass::Pass(Diligent::IRenderDevice& device,
           const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
           MaterialPass::type passId)
    : pso{},
      id{passId}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakeDefaultGraphicsPipelineCreateInfo(Diligent::ISwapChain& swapChain,
                                           Diligent::IShader& vertexShader,
                                           Diligent::IShader& pixelShader,
                                           std::span<Diligent::IPipelineResourceSignature*> signatures,
                                           std::span<const Diligent::LayoutElement> layoutElements,
                                           std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo
{
    using namespace Diligent;

    auto ci = GraphicsPipelineStateCreateInfo{};
    ci.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    ci.PSODesc.Name = name.data();
    ci.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    ci.ppResourceSignatures = signatures.data();
    ci.ResourceSignaturesCount = static_cast<uint32_t>(signatures.size());

    ci.pVS = &vertexShader;
    ci.pPS = &pixelShader;

    ci.GraphicsPipeline.NumRenderTargets             = 1;
    ci.GraphicsPipeline.RTVFormats[0]                = swapChain.GetDesc().ColorBufferFormat;
    ci.GraphicsPipeline.DSVFormat                    = swapChain.GetDesc().DepthBufferFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return ci;
}

auto MakeTestPass(Diligent::IRenderDevice& device,
                  Diligent::ISwapChain& swapChain,
                  ShaderFactory& shaderFactory,
                  Diligent::IPipelineResourceSignature& globalSignature,
                  Diligent::IPipelineResourceSignature& componentSignature,
                  Diligent::IPipelineResourceSignature& materialSignature) -> Pass
{
    auto vertexShader = shaderFactory.MakeShaderFromSource(
        std::span{g_vertexShader},
        "Cube VS",
        Diligent::SHADER_TYPE_VERTEX
    );

    auto pixelShader = shaderFactory.MakeShaderFromSource(
        std::span{g_pixelShader},
        "Cube PS",
        Diligent::SHADER_TYPE_PIXEL
    );

    auto signatures = std::array{&globalSignature, &componentSignature, &materialSignature};
    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeDefaultGraphicsPipelineCreateInfo(
        swapChain,
        *vertexShader,
        *pixelShader,
        signatures,
        layoutElements,
        "Test PSO"
    );

    return Pass(device, createInfo, MaterialPass::Toon);
}

auto MakePasses(Diligent::IRenderDevice& device,
                Diligent::ISwapChain& swapChain,
                ShaderFactory& shaderFactory,
                ShaderBindings& shaderBindings) -> std::vector<Pass>
{
    return std::vector<Pass>{
        MakeTestPass(
            device,
            swapChain,
            shaderFactory,
            shaderBindings.GetGlobalSignature().GetResourceSignature(),
            shaderBindings.GetComponentSignature().GetResourceSignature(),
            shaderBindings.GetMaterialSignature().GetResourceSignature()
        )
    };
}
} // namespace nc::graphics
