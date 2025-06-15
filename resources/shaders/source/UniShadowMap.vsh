#include "core/PerFrameTypes.fxh"
#include "core/PerPassTypes.fxh"
#include "core/Lighting.fxh"

struct VSInput
{
    float3 Pos : ATTRIB0;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID, uint VertexID : SV_VertexID, out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(TransformedPos, LightMatrices[light.lightMatrixIndex].viewProjection);
}
