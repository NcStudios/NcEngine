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
    uint   MaterialIndex;
    float4 WorldPos;
    float3 LocalPos;
};

/*
struct ShapeKeyMetadata
{
    int ShapeKeyAnimationIndex;
    float DurationInSeconds;
    uint NumShapeKeys;
};

Texture2D     ShapeKeyAnims[];
SamplerState  ShapeKeyAnims_sampler; // By convention, texture samplers must use the '_sampler' suffix

*/

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID, uint VertexID : SV_VertexID, out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    uint materialIndex = StaticInstances[InstanceID].materialIndex;
    float4 TransformedPos = mul(float4(VSIn.Pos, 1.0), Transforms[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize( mul(float4(VSIn.Normal, 0.0), Transforms[transformIndex].model));
    PSIn.WorldPos = TransformedPos;
    PSIn.LocalPos = VSIn.Pos.xyz;
    PSIn.MaterialIndex = materialIndex;

    /*
    if (ShapeKeyMetadata.ShapeKeyAnimationIndex > -1)
    {
        get time.deltatime
        Get ShapeKeyMetadata.DurationInSeconds;
        If TotalT = 10 seconds
        If T = 3 seconds
        



        uint2 shapeKeyUVA = uint2{0, ShapeKeyIndex}
        float3 shapeKeyA = ShapeKeyAnims[ShapeKeyAnimationIndex].Sample(ShapeKeyAnims_sampler, PSIn.UV * hatchTiling).b;
    }
    */
}
