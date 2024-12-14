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


// temp
#define ENABLE_SKINNING 1

#ifdef ENABLE_SKINNING

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
};

StructuredBuffer<SkinnedMeshInstanceData> SkinnedInstanceBufferData;

#define INSTANCE_DATA SkinnedMeshInstanceData
#define INSTANCE_BUFFER SkinnedInstanceBufferData

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

StructuredBuffer<BoneData> BoneBufferData;

bool IsValidBoneIndex(uint boneIndex)
{
    return boneIndex != 4294967295;
}

bool IsValidAnimationTransform(float4x4 mat)
{
    // Check for zero matrix, ignoring homogenous coord
    return all(mat[0] != 0.0) ||
           all(mat[1] != 0.0) ||
           all(mat[2] != 0.0) ||
           mat[3].xyz != float3(0.0, 0.0, 0.0);
}

float4x4 CombineBoneMatrices(uint base, uint4 boneOffsets, float4 boneWeights)
{
    float4x4 boneTransform = float4x4(0.0);
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0f)
        {
            boneTransform += BoneBufferData[base + boneOffsets[i]].animatedBoneMatrix * boneWeights[i];
        }
    }

    return boneTransform;
}

#else

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstanceBufferData;

#define INSTANCE_DATA StaticMeshInstanceData
#define INSTANCE_BUFFER StaticInstanceBufferData

#endif // ENABLE_SKINNING

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint dirLightsCount;
    uint pointLightsCount;
    uint spotLightsCount;
    float2 padding;
};

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSInput PSIn)
{
    INSTANCE_DATA instance = INSTANCE_BUFFER[InstanceID];
    float4 pos = float4(VSIn.Pos, 1.0);
    float3 normal = VSIn.Normal;

#ifdef ENABLE_SKINNING
    if (IsValidBoneIndex(instance.boneIndex))
    {
        float4x4 animatedTransform = CombineBoneMatrices(instance.boneIndex, VSIn.BoneIds, VSIn.BoneWeights);
        if (IsValidAnimationTransform(animatedTransform))
        {
            pos = mul(pos, animatedTransform);
            normal = mul(animatedTransform, normal); // ask Jare what this should be like
        }
    }
#endif // ENABLE_SKINNING

    uint transformIndex = instance.transformIndex;
    float4 worldPos = mul(pos, TransformBufferData[transformIndex].modelMatrix);
    PSIn.Pos = mul(worldPos, cameraViewProjection);
    PSIn.UV = VSIn.UV;
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = worldPos.xyz;
    PSIn.MaterialIndex = instance.materialIndex;
}
)"};
} // anonymous namespace

namespace nc::graphics
{
Pass::Pass(Diligent::IRenderDevice& device,
           const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
           MaterialPassFlag::type passId)
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

    return Pass(device, createInfo, MaterialPassFlag::Toon);
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

    return Pass(device, createInfo, MaterialPassFlag::Toon);
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
