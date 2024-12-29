struct VSInput
{
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
};

struct PSOutput
{
    float4 Pos           : SV_POSITION;
    float2 UV            : TEX_COORD;
    uint   TextureIndex;
};

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
};

struct ParticleData
{
    float4x4 model;
    uint textureIndex;
};

StructuredBuffer<ParticleData> ParticleBufferData;

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSOutput PSOut)
{
    ParticleData particle = ParticleBufferData[InstanceID];
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), particle.model);
    PSOut.Pos = mul(TransformedPos, cameraViewProjection);
    PSOut.UV = VSIn.UV;
    PSOut.TextureIndex = particle.textureIndex;
}
