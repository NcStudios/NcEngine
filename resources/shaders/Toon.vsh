struct VSInput
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
    float4x4 model;
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
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), TransformBufferData[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].model, VSIn.Normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = TransformedPos.xyz;
    PSIn.MaterialIndex = materialIndex;
}
