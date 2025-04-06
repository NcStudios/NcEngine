#include "core/PerFrameTypes.fxh"
#include "core/PerPassTypes.fxh"
#include "core/Lighting.fxh"

struct VSInput
{
    float3 Pos    : ATTRIB0;
};

struct PSInput 
{
    float4 Pos : SV_POSITION;
    float4 WorldPos;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    PSIn.WorldPos = TransformedPos;

    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(TransformedPos, LightMatrices[light.lightMatrixIndex  + lightFaceIndex].viewProjection);
}
