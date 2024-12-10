#include "Pass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/resource/MeshBuffer.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

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

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstanceBufferData;

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
    uint transformIndex = StaticInstanceBufferData[InstanceID].transformIndex;
    uint materialIndex = StaticInstanceBufferData[InstanceID].materialIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), TransformBufferData[transformIndex].modelMatrix);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, VSIn.Normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = TransformedPos.xyz;
    PSIn.MaterialIndex = materialIndex;
}
)"};

constexpr auto g_pixelShaderSkinned = g_pixelShader;

constexpr auto g_vertexShaderSkinned = std::string_view{
R"(struct VSInput
{
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

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
};

StructuredBuffer<SkinnedMeshInstanceData> SkinnedInstanceBufferData;

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

StructuredBuffer<BoneData> BoneBufferData;

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint dirLightsCount;
    uint pointLightsCount;
    uint spotLightsCount;
    float2 padding;
};

bool IsMatrixNonZero(float4x4 mat)
{
    return dot(mat[0], float4(1.0)) + dot(mat[1], float4(1.0)) +
            dot(mat[2], float4(1.0)) + dot(mat[3], float4(1.0)) > 0.0;
}

// Super icky, unused offsets are set w/ homogenous extension on translation
bool IsMatrixNonZeroHom(float4x4 mat)
{
    // Check the first three rows for being completely zero
    bool rowsZero = 
        all(mat[0] == float4(0.0, 0.0, 0.0, 0.0)) &&
        all(mat[1] == float4(0.0, 0.0, 0.0, 0.0)) &&
        all(mat[2] == float4(0.0, 0.0, 0.0, 0.0));

    // Check the last row for [0, 0, 0, 1]
    bool lastRowValid = all(mat[3] == float4(0.0, 0.0, 0.0, 1.0));

    return !(rowsZero && lastRowValid);
}

float4x4 ApplyAnimation(uint base, uint4 boneOffsets, float4 boneWeights)
{
    float4x4 boneTransform = float4x4(0.0);

    // todo: why are we checking for nonzero matrix? having a hard time finding case where matrix is zero


    if (boneWeights[0] > -1.0f)
    {
        boneTransform += BoneBufferData[base + boneOffsets[0]].animatedBoneMatrix * boneWeights[0];
    }
    if (boneWeights[1] > -1.0f)
    {
        boneTransform += BoneBufferData[base + boneOffsets[1]].animatedBoneMatrix * boneWeights[1];
    }
    if (boneWeights[2] > -1.0f)
    {
        boneTransform += BoneBufferData[base + boneOffsets[2]].animatedBoneMatrix * boneWeights[2];
    }
    if (boneWeights[3] > -1.0f)
    {
        boneTransform += BoneBufferData[base + boneOffsets[3]].animatedBoneMatrix * boneWeights[3];
    }

    // if (boneWeights[0] > -1.0f && IsMatrixNonZeroHom(BoneBufferData[base + boneOffsets[0]].animatedBoneMatrix))
    // {
    //     boneTransform += BoneBufferData[base + boneOffsets[0]].animatedBoneMatrix * boneWeights[0];
    // }
    // if (boneWeights[1] > -1.0f && IsMatrixNonZeroHom(BoneBufferData[base + boneOffsets[1]].animatedBoneMatrix))
    // {
    //     boneTransform += BoneBufferData[base + boneOffsets[1]].animatedBoneMatrix * boneWeights[1];
    // }
    // if (boneWeights[2] > -1.0f && IsMatrixNonZeroHom(BoneBufferData[base + boneOffsets[2]].animatedBoneMatrix))
    // {
    //     boneTransform += BoneBufferData[base + boneOffsets[2]].animatedBoneMatrix * boneWeights[2];
    // }
    // if (boneWeights[3] > -1.0f && IsMatrixNonZeroHom(BoneBufferData[base + boneOffsets[3]].animatedBoneMatrix))
    // {
    //     boneTransform += BoneBufferData[base + boneOffsets[3]].animatedBoneMatrix * boneWeights[3];
    // }

    return boneTransform;
}

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    float4x4 boneTransform = ApplyAnimation(SkinnedInstanceBufferData[InstanceID].boneIndex, VSIn.BoneIds, VSIn.BoneWeights);
    float4 animatedPos = float4(VSIn.Pos, 1.0);
    float3 animatedNormal = VSIn.Normal;
    if (IsMatrixNonZeroHom(boneTransform))
    {
        animatedPos = mul(animatedPos, boneTransform);
        animatedNormal = mul(animatedNormal, boneTransform); // ask Jare what this should be like
    }


    uint transformIndex = SkinnedInstanceBufferData[InstanceID].transformIndex;
    uint materialIndex = SkinnedInstanceBufferData[InstanceID].materialIndex;
    float4 TransformedPos = mul(animatedPos, TransformBufferData[transformIndex].modelMatrix);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    // PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, VSIn.Normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, animatedNormal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = TransformedPos.xyz;
    PSIn.MaterialIndex = materialIndex;
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
                  Diligent::IPipelineResourceSignature& perFrameResourceSignature) -> Pass
{
    auto vertexShader = shaderFactory.MakeShaderFromSource(
        std::span{g_vertexShader},
        "Cube VS",
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromSource(
        std::span{g_pixelShader},
        "Cube PS",
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto signatures = std::array{&perFrameResourceSignature};
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

auto MakeTestSkinnedPass(Diligent::IRenderDevice& device,
                  Diligent::ISwapChain& swapChain,
                  ShaderFactory& shaderFactory,
                  Diligent::IPipelineResourceSignature& perFrameResourceSignature) -> Pass
{
    auto vertexShader = shaderFactory.MakeShaderFromSource(
        std::span{g_vertexShaderSkinned},
        "Cube VS - Skinned",
        Diligent::SHADER_TYPE_VERTEX,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto pixelShader = shaderFactory.MakeShaderFromSource(
        std::span{g_pixelShaderSkinned},
        "Cube PS - Skinned",
        Diligent::SHADER_TYPE_PIXEL,
        Diligent::SHADER_SOURCE_LANGUAGE_HLSL
    );

    auto signatures = std::array{&perFrameResourceSignature};
    auto layoutElements = GetMeshVertexLayoutElements(0);
    auto createInfo = MakeDefaultGraphicsPipelineCreateInfo(
        swapChain,
        *vertexShader,
        *pixelShader,
        signatures,
        layoutElements,
        "Test PSO - Skinned"
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
            shaderBindings.GetPerFrameSignature().GetResourceSignature()
        )
    };
}

auto MakeSkinnedPasses(Diligent::IRenderDevice& device,
                       Diligent::ISwapChain& swapChain,
                       ShaderFactory& shaderFactory,
                       ShaderBindings& shaderBindings)-> std::vector<Pass>
{
    return std::vector<Pass>{
        MakeTestSkinnedPass(
            device,
            swapChain,
            shaderFactory,
            shaderBindings.GetPerFrameSignature().GetResourceSignature()
        )
    };
}
} // namespace nc::graphics
