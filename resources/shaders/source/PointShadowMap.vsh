#include "core/PerFrameTypes.fxh"
#include "core/PerPassTypes.fxh"
#include "core/Lighting.fxh"
#include "core/ShapeKeyAnimation.fxh"

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

Texture2D<float> ShapeKeyClips[];
SamplerState  Textures_sampler; 
StructuredBuffer<ShapeKeyMetadata> ShapeKeyAnimationMetadata;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID, uint VertexID : SV_VertexID, out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;

    float3 animatedPos = VSIn.Pos;

    uint shapeKeyMetadataIndex = StaticInstances[InstanceID].shapeKeyMetadataIndex;
    if (IsValidShapeKeyIndex(shapeKeyMetadataIndex))
    {
        ShapeKeyMetadata metadata = ShapeKeyAnimationMetadata[shapeKeyMetadataIndex];
        animatedPos += ApplyShapeKeyAnimation(shapeKeyMetadataIndex, metadata, ShapeKeyClips[metadata.shapeKeyAnimationIndex], time, VertexID - StaticInstances[InstanceID].vertexOffset);
    }
    
    float4 TransformedPos = mul(float4(animatedPos, 1.0), Transforms[transformIndex].model);
    PSIn.WorldPos = TransformedPos;

    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(TransformedPos, LightMatrices[light.lightMatrixIndex  + lightFaceIndex].viewProjection);
}
