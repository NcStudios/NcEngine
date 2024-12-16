#include "Lighting.fxh"

struct VSInput
{
    // Vertex attributes
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
};

struct PSInput 
{
    float4 Pos : SV_POSITION;
};

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint dirLightsCount;
    uint pointLightsCount;
    uint spotLightsCount;
    float2 padding;
};

cbuffer PerPassInstanceBufferData
{
    uint colorRT1;
    uint colorRT2;
    uint colorRT3;
    uint colorRT4;
    uint depthRT1;
    uint depthRT2;
    uint depthRT3;
    uint depthRT4;
    uint spotLightIndex;
};

struct TransformData
{
    float4x4 model;
};

StructuredBuffer<TransformData> TransformBufferData;
StructuredBuffer<SpotLightData> SpotLightBufferData;

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstanceBufferData;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstanceBufferData[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), TransformBufferData[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, SpotLightBufferData[spotLightIndex].viewProj);
}
