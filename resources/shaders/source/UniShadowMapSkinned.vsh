#include "core/PerFrameTypes.fxh"
#include "core/PerPassTypes.fxh"
#include "core/Lighting.fxh"

struct VSInput
{
    float3 Pos         : ATTRIB0;
    float4 BoneWeights : ATTRIB1;
    uint4  BoneIds     : ATTRIB2;
};

struct PSInput
{
    float4 Pos           : SV_POSITION;
    float3 Normal        : NORMAL;
    float2 UV            : TEX_COORD;
    uint   MaterialIndex;
    float3 WorldPos;
    float3 LocalPos;
};

StructuredBuffer<TransformData> Transforms;

// todo: #802 Define this at compile time
#define ENABLE_SKINNING 1

#ifdef ENABLE_SKINNING
StructuredBuffer<SkinnedMeshInstanceData> SkinnedInstances;
StructuredBuffer<BoneData> Bones;

#define INSTANCE_DATA SkinnedMeshInstanceData
#define INSTANCE_BUFFER SkinnedInstances

#include "core/SkeletalAnimation.fxh"
#else
StructuredBuffer<StaticMeshInstanceData> StaticInstances;

#define INSTANCE_DATA StaticMeshInstanceData
#define INSTANCE_BUFFER StaticInstances
#endif // ENABLE_SKINNING

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, uint VertexID : SV_VertexID, out PSInput PSIn)
{
    INSTANCE_DATA instance = INSTANCE_BUFFER[InstanceID];
    float4 pos = float4(VSIn.Pos, 1.0);

#ifdef ENABLE_SKINNING
    if (IsValidBoneIndex(instance.boneIndex))
    {
        float4x4 animatedTransform = CombineBoneMatrices(instance.boneIndex, VSIn.BoneIds, VSIn.BoneWeights);
        if (IsValidAnimationTransform(animatedTransform))
        {
            pos = mul(pos, animatedTransform);
        }
    }
#endif // ENABLE_SKINNING

    uint transformIndex = instance.transformIndex;
    float4 worldPos = mul(pos, Transforms[transformIndex].model);

    LightData light = Lights[lightIndex];
    PSIn.Pos = mul(worldPos, LightMatrices[light.lightMatrixIndex].viewProjection);
}
