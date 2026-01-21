#include "core/PerFrameTypes.fxh"

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
    uint   MaterialIndex : MAT_INDEX;
    float4 WorldPos      : WORLD_POS;
    float3 LocalPos      : LOCAL_POS;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    uint materialIndex = StaticInstances[InstanceID].materialIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize( mul(float4(VSIn.Normal, 0.0), Transforms[transformIndex].model)).xyz;
    PSIn.WorldPos = TransformedPos;
    PSIn.LocalPos = VSIn.Pos.xyz;
    PSIn.MaterialIndex = materialIndex;
}
