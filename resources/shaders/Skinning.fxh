// todo: #802 Define this at compile time
#define ENABLE_SKINNING 1

#ifdef ENABLE_SKINNING

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
};

StructuredBuffer<SkinnedMeshInstanceData> SkinnedInstanceBufferData;

#define INSTANCE_DATA SkinnedMeshInstanceData
#define INSTANCE_BUFFER SkinnedInstanceBufferData

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

StructuredBuffer<BoneData> BoneBufferData;

bool IsValidBoneIndex(uint boneIndex)
{
    return boneIndex != 4294967295;
}

bool IsValidAnimationTransform(float4x4 mat)
{
    // Check for zero matrix, ignoring homogenous coord
    return all(mat[0] != 0.0) ||
           all(mat[1] != 0.0) ||
           all(mat[2] != 0.0) ||
           mat[3].xyz != float3(0.0, 0.0, 0.0);
}

float4x4 CombineBoneMatrices(uint base, uint4 boneOffsets, float4 boneWeights)
{
    float4x4 boneTransform = float4x4(0.0);
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0f)
        {
            boneTransform += BoneBufferData[base + boneOffsets[i]].animatedBoneMatrix * boneWeights[i];
        }
    }

    return boneTransform;
}

#else

struct StaticMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
};

StructuredBuffer<StaticMeshInstanceData> StaticInstanceBufferData;

#define INSTANCE_DATA StaticMeshInstanceData
#define INSTANCE_BUFFER StaticInstanceBufferData

#endif // ENABLE_SKINNING
