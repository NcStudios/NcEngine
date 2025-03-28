struct VSInput
{
    float3 Pos         : ATTRIB0;
    float3 Normal      : ATTRIB1;
    float2 UV          : ATTRIB2;
    float3 Tangent     : ATTRIB3;
    float3 Bitangent   : ATTRIB4;
    float4 BoneWeights : ATTRIB5;
    uint4  BoneIds     : ATTRIB6;
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

struct TransformData
{
    float4x4 modelMatrix;
};

StructuredBuffer<TransformData> Transforms;

// todo: #802 Define this at compile time
#define ENABLE_SKINNING 1

#ifdef ENABLE_SKINNING

struct SkinnedMeshInstanceData
{
    uint transformIndex;
    uint materialIndex;
    uint boneIndex;
};

StructuredBuffer<SkinnedMeshInstanceData> SkinnedInstances;

#define INSTANCE_DATA SkinnedMeshInstanceData
#define INSTANCE_BUFFER SkinnedInstances

struct BoneData
{
    float4x4 animatedBoneMatrix;
};

StructuredBuffer<BoneData> Bones;

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
            boneTransform += Bones[base + boneOffsets[i]].animatedBoneMatrix * boneWeights[i];
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

StructuredBuffer<StaticMeshInstanceData> StaticInstances;

#define INSTANCE_DATA StaticMeshInstanceData
#define INSTANCE_BUFFER StaticInstances

#endif // ENABLE_SKINNING

cbuffer EnvironmentProperties
{
    float4x4 cameraViewProjection;
    float4x4 cameraInvProjection;
    float3 cameraPosition;
    uint lightCount;
    float nearClip;
    float farClip;
};

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSInput PSIn)
{
    INSTANCE_DATA instance = INSTANCE_BUFFER[InstanceID];
    float4 pos = float4(VSIn.Pos, 1.0);
    float4 normal = float4(VSIn.Normal, 0.0);

#ifdef ENABLE_SKINNING
    if (IsValidBoneIndex(instance.boneIndex))
    {
        float4x4 animatedTransform = CombineBoneMatrices(instance.boneIndex, VSIn.BoneIds, VSIn.BoneWeights);
        if (IsValidAnimationTransform(animatedTransform))
        {
            pos = mul(pos, animatedTransform);
            normal = mul(animatedTransform, normal);
        }
    }
#endif // ENABLE_SKINNING

    uint transformIndex = instance.transformIndex;
    float4 worldPos = mul(pos, Transforms[transformIndex].modelMatrix);
    PSIn.Pos = mul(worldPos, cameraViewProjection);
    PSIn.UV = VSIn.UV;
    PSIn.Normal = normalize(mul(normal, Transforms[transformIndex].modelMatrix));
    PSIn.LocalPos = VSIn.Pos.xyz;
    PSIn.WorldPos = worldPos;
    PSIn.MaterialIndex = instance.materialIndex;
}
