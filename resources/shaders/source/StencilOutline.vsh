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
    uint   MaterialIndex : MAT_INDEX;
    float2 UV            : TEX_COORD;
};

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;
StructuredBuffer<MaterialData> Materials : register(t1);

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID,  out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    uint materialIndex = StaticInstances[InstanceID].materialIndex;
    MaterialData material = Materials[materialIndex];
    PSIn.UV  = VSIn.UV;

    // Scale vertex position along normals
    float4 scaledPos = float4(VSIn.Pos, 1.0f) + normalize(float4(VSIn.Normal, 0.0f)) * material.stencilOutlineWidth;

    float4 TransformedPos = mul(scaledPos, Transforms[transformIndex].model);
    PSIn.MaterialIndex = materialIndex;
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
}
