#include "Lighting.fxh"

struct VSInput
{
    // Vertex attributes
    float3 Pos    : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV     : ATTRIB2;
};

struct PSInput 
{
    float4 Pos : SV_POSITION;
    float4 WorldPos;
};

struct TransformData
{
    float4x4 model;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<LightData> Lights;
StructuredBuffer<LightMatrix> LightMatrices;

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstances;

cbuffer EnvironmentProperties
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
};

cbuffer SinkIndices
{
    int colorRT1;
    int colorRT2;
    int colorRT3;
    int colorRT4;
    int depthRT1;
    int depthRT2;
    int depthRT3;
    uint hasPostProcess;
    uint lightIndex;
    uint lightFaceIndex;
};

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    PSIn.WorldPos = TransformedPos;

    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(TransformedPos, LightMatrices[light.lightMatrixIndex  + lightFaceIndex].viewProjection);
}
