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
    float3 WorldPos;
};

struct TransformData
{
    float4x4 modelMatrix;
};

StructuredBuffer<TransformData> TransformBufferData;

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

cbuffer EnvironmentBufferData
{
    float4x4 cameraViewProjection;
    float3 cameraPosition;
    uint lightCount;
};

void main(in VSInput VSIn, uint InstanceID : SV_InstanceID, out PSInput PSIn)
{
    INSTANCE_DATA instance = INSTANCE_BUFFER[InstanceID];
    float4 pos = float4(VSIn.Pos, 1.0);
    float3 normal = VSIn.Normal;

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
    float4 worldPos = mul(pos, TransformBufferData[transformIndex].modelMatrix);
    PSIn.Pos = mul(worldPos, cameraViewProjection);
    PSIn.UV = VSIn.UV;
    PSIn.Normal = normalize(mul(TransformBufferData[transformIndex].modelMatrix, normal)); // @TODO #805, compute inverse model matrix CPU-side
    PSIn.WorldPos = worldPos.xyz;
    PSIn.MaterialIndex = instance.materialIndex;
}
