#include "core/PerFrameTypes.fxh"
#include "core/Util.fxh"

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
};

SamplerState  ShapeKeyAnims_sampler; // By convention, texture samplers must use the '_sampler' suffix

struct ShapeKeyMetadata
{
    int shapeKeyAnimationIndex;
    float durationInSeconds;
    uint numShapeKeys;
    uint padding;
};

*/
Texture2D<float> ShapeKeyClips[];
SamplerState  Textures_sampler; // By convention, texture samplers must use the '_sampler' suffix
StructuredBuffer<ShapeKeyMetadata> ShapeKeyAnimationMetadata;

StructuredBuffer<TransformData> Transforms;
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

void main(in  VSInput VSIn, uint InstanceID : SV_InstanceID, uint VertexID : SV_VertexID, out PSInput PSIn)
{
    uint transformIndex = StaticInstances[InstanceID].transformIndex;
    uint materialIndex = StaticInstances[InstanceID].materialIndex;

    float3 animatedPos = VSIn.Pos;

    uint shapeKeyMetadataIndex = StaticInstances[InstanceID].shapeKeyMetadataIndex;
    if (IsValidShapeKeyIndex(shapeKeyMetadataIndex))
    {
        ShapeKeyMetadata metadata = ShapeKeyAnimationMetadata[shapeKeyMetadataIndex];
        // float shapeKeyPosition = (time) % metadata.durationInSeconds;
        // uint shapeKeyIndexLow = floor(shapeKeyPosition); // 1001 % 10 second clip = 1, 1002 % 10 = 2
        // uint shapeKeyIndexHigh = ceil(shapeKeyPosition);
        // float shapeKeyLerpFactor = shapeKeyPosition % 1;


        float wrappedTime = fmod(time, metadata.durationInSeconds);
        float normalizedT = wrappedTime / metadata.durationInSeconds;
        float frameF = normalizedT * (metadata.numShapeKeys - 1);
        uint shapeKeyIndexLow = (uint)floor(frameF);
        uint shapeKeyIndexHigh = min(shapeKeyIndexLow + 1, metadata.numShapeKeys - 1);
        float shapeKeyLerpFactor = frac(frameF);



        int3 texelCoords = int3(VertexID * 3 + 0, shapeKeyIndexLow, 0); // X
        float positionOffsetLowX = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords); // X, Y, Z, X, Y, Z, X, Y, Z
        texelCoords.x += 1; // Y
        float positionOffsetLowY = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords);
        texelCoords.x += 1; // Z
        float positionOffsetLowZ = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords);

        texelCoords = int3(VertexID * 3 + 0, shapeKeyIndexHigh, 0); // X
        float positionOffsetHighX = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords); // X, Y, Z, X, Y, Z, X, Y, Z
        texelCoords.x += 1; // Y
        float positionOffsetHighY = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords);
        texelCoords.x += 1; // Z
        float positionOffsetHighZ = ShapeKeyClips[metadata.shapeKeyAnimationIndex].Load(texelCoords);

        float3 positionOffset = float3(lerp(positionOffsetLowX, positionOffsetHighX, shapeKeyLerpFactor), lerp(positionOffsetLowY, positionOffsetHighY, shapeKeyLerpFactor), lerp(positionOffsetLowZ, positionOffsetHighZ, shapeKeyLerpFactor));
        animatedPos += positionOffset;
    }

    float4 TransformedPos = mul(float4(animatedPos, 1.0), Transforms[transformIndex].model);
    PSIn.Pos = mul(TransformedPos, cameraViewProjection);
    PSIn.UV  = VSIn.UV;
    PSIn.Normal = normalize( mul(float4(VSIn.Normal, 0.0), Transforms[transformIndex].model));
    PSIn.WorldPos = TransformedPos;
    PSIn.LocalPos = animatedPos.xyz;
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
