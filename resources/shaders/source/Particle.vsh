#include "core/PerFrameTypes.fxh"

struct VSInput
{
    float3 Pos : POSITION;
    float2 UV  : TEXCOORD0;
};

struct PSOutput
{
    float4 Pos           : SV_POSITION;
    float2 UV            : TEX_COORD;
    float4 Color         : COLOR;
    uint   TextureIndex  : TEX_INDEX;
};

StructuredBuffer<ParticleData> Particles;

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSOutput PSOut)
{
    ParticleData particle = Particles[InstanceID];
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), particle.model);
    PSOut.Pos = mul(TransformedPos, cameraViewProjection);
    PSOut.UV = VSIn.UV;
    PSOut.Color = particle.color;
    PSOut.TextureIndex = particle.textureIndex;
}
