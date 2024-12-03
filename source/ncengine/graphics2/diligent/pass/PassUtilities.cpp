#include "PassUtilities.h"
#include "graphics2/diligent/resource/MeshBuffer.h"

#include <array>
#include <span>

namespace
{
constexpr auto g_pixelShader = std::string_view{
R"(
#ifdef VULKAN
// NonUniformResourceIndex is not supported by GLSLang
#   define NonUniformResourceIndex(x) x
#endif

#include "Lighting.fxh"

Texture2D     TextureBufferData[];
SamplerState  TextureBufferData_sampler; // By convention, texture samplers must use the '_sampler' suffix

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint dirLightsCount;
    uint pointLightsCount;
    uint spotLightsCount;
    float2 padding;
};

struct MaterialData
{
    float3 gradientStart;
    uint diffuseTexture;
    float3 gradientEnd;
    uint normalIndex;
    float3 outlineColor;
    float outlineWidth;
};

StructuredBuffer<MaterialData> MaterialBufferData : register(t1);
StructuredBuffer<DirectionalLightData> DirectionalLightBufferData : register(t2);
StructuredBuffer<PointLightData> PointLightBufferData : register(t3);
StructuredBuffer<SpotLightData> SpotLightBufferData : register(t4);

struct PSInput 
{ 
    float4 Pos           : SV_POSITION;
    float3 Normal        : NORMAL;
    float2 UV            : TEX_COORD; 
    uint   MaterialIndex;
    float3 WorldPos;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};


void main(in  PSInput  PSIn, out PSOutput PSOut)
{
    float4 Color;
    uint TexIndex = MaterialBufferData[PSIn.MaterialIndex].diffuseTexture;
    Color = TextureBufferData[TexIndex].Sample(TextureBufferData_sampler, PSIn.UV);
    float alpha = Color.a;
    float3 finalColor = {0.0f, 0.0f, 0.0f};

    for (int i = 0; i < dirLightsCount; i++)
    {
        LightInfluence influence = DirectionalLightRadiance(DirectionalLightBufferData[i], PSIn.WorldPos, cameraPosition, PSIn.Normal);
        finalColor += influence.color * influence.specularAmt + influence.color * influence.diffuseAmt;
    }
    for (int i = 0; i < pointLightsCount; i++)
    {
        LightInfluence influence = PointLightRadiance(PointLightBufferData[i], PSIn.WorldPos, cameraPosition, PSIn.Normal);
        finalColor += influence.color * influence.specularAmt + influence.color * influence.diffuseAmt;
    }
    for (int i = 0; i < spotLightsCount; i++)
    {
        LightInfluence influence = SpotLightRadiance(SpotLightBufferData[i], PSIn.WorldPos, cameraPosition, PSIn.Normal);
        finalColor += influence.color * influence.specularAmt + influence.color * influence.diffuseAmt;
    }

    Color *= float4(finalColor, 1.0);
    Color.a = alpha;
    PSOut.Color = Color;
}
)"};

constexpr auto g_vertexShader = std::string_view{
R"(struct VSInput
{
    // Vertex attributes
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
};

struct PSInput 
{
    float4 Pos           : SV_POSITION;
    float3 Normal        : NORMAL;
    float2 UV            : TEX_COORD;
    uint   MaterialIndex;
    float3 WorldPos;
};

struct TransformData
{
    float4x4 modelMatrix;
};

StructuredBuffer<TransformData> TransformBufferData;

struct InstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<InstanceData> InstanceBufferData;

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint dirLightsCount;
    uint pointLightsCount;
    uint spotLightsCount;
    float2 padding;
};

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = InstanceBufferData[InstanceID].transformIndex;
    uint materialIndex = InstanceBufferData[InstanceID].materialIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), TransformBufferData[transformIndex].modelMatrix);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, VSIn.Normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = TransformedPos.xyz;
    PSIn.MaterialIndex = materialIndex;
}
)"};
} // anonymous namespace

namespace nc::graphics
{
auto MakeOffScreenPipelineCreateInfo(Diligent::IShader& vertexShader,
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
    ci.GraphicsPipeline.RTVFormats[0]                = OffScreenColorRTFormat;
    ci.GraphicsPipeline.DSVFormat                    = OffScreenDepthRTFormat;
    ci.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ci.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    ci.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    ci.GraphicsPipeline.InputLayout.LayoutElements   = layoutElements.data();
    ci.GraphicsPipeline.InputLayout.NumElements      = static_cast<uint32_t>(layoutElements.size());

    return ci;
}

auto MakeOffScreenMaterialPass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               Diligent::IPipelineResourceSignature& perFrameResourceSignature,
                               PostProcessSinkBufferResource& postProcessBufferResource,
                               std::string_view vertexShaderName,
                               std::string_view pixelShaderName,
                               std::string_view pipelineName) -> MaterialPass
{
    auto vertexShader = shaderFactory.MakeShaderFromSource(
        std::span{g_vertexShader},
        vertexShaderName.data(),
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromSource(
        std::span{g_pixelShader},
        pixelShaderName.data(),
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto signatures = std::array{&perFrameResourceSignature};

    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeOffScreenPipelineCreateInfo(
        *vertexShader,
        *pixelShader,
        signatures,
        layoutElements,
        pipelineName.data()
    );

    auto renderTargetIndices = postProcessBufferResource.Add(device, 1, 1, swapChain.GetDesc().Width, swapChain.GetDesc().Height);

    return MaterialPass(device, createInfo, MaterialPassFlag::Toon, renderTargetIndices[0], renderTargetIndices[1]);
}

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessSinkBufferResource& postProcessBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex)
{
    Diligent::ITextureView* pRTV = nullptr;
    Diligent::ITextureView* pDSV = nullptr;

    if (colorRenderTargetIndex == SwapChainColorRTIndex)
    {
        pRTV = swapChain.GetCurrentBackBufferRTV();
    }
    else
    {
        pRTV = static_cast<Diligent::ITextureView*>(postProcessBufferResource.GetColorRenderTarget(colorRenderTargetIndex));
    }

    if (depthRenderTargetIndex == SwapChainColorRTIndex)
    {
        pDSV = swapChain.GetDepthBufferDSV();
    }
    else
    {
        pDSV = static_cast<Diligent::ITextureView*>(postProcessBufferResource.GetDepthRenderTarget(depthRenderTargetIndex));
    }

    context.SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    constexpr auto ClearColor = nc::Vector4{0.050f, 0.050f, 0.050f, 1.0f};
    context.ClearRenderTarget(pRTV, &ClearColor.x, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    context.ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool
{
    return (colorRenderTargetIndex != SwapChainColorRTIndex || depthRenderTargetIndex != SwapChainDepthRTIndex);
}
} // namespace nc::graphics
