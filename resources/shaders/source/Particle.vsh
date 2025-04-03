#include "core/PerFrameTypes.fxh"

struct VSInput
{
    float3 Pos         : ATTRIB0;
    float2 UV          : ATTRIB1;
};

struct PSOutput
{
    float4 Pos           : SV_POSITION;
    float2 UV            : TEX_COORD;
    uint   TextureIndex;
};

StructuredBuffer<ParticleData> Particles;

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSOutput PSOut)
{
    ParticleData particle = Particles[InstanceID];
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), particle.model);
    PSOut.Pos = mul(TransformedPos, cameraViewProjection);
    PSOut.UV = VSIn.UV;
    PSOut.TextureIndex = particle.textureIndex;
}
