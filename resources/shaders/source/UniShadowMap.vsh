#include "core/ShaderTypes.fxh"
#include "core/Lighting.fxh"

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
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<LightData> Lights;
StructuredBuffer<LightMatrix> LightMatrices;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(TransformedPos, LightMatrices[light.lightMatrixIndex].viewProjection);
}
