#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    mat4 model;

    // Textures
    uint unused1;
    uint outlineWidthPercentage;
    uint unused2;
    uint unused3;

    uint skeletalAnimationIndex;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    mat4 cameraViewProjection;
    vec4 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in vec4 inBoneWeights;
layout (location = 6) in ivec4 inBoneIDs;

struct SkeletalAnimationData
{
    mat4 finalTransform;
};

layout (std140, set=0, binding=6) readonly buffer AnimatedBoneTransforms
{
    SkeletalAnimationData animatedBones[];
} skeletalAnimationBuffer;

mat4 ApplyAnimation(uint animIndex)
{
    mat4 boneTransform = mat4(0.0);

    if (skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[0]].finalTransform != mat4(0.0) && inBoneWeights[0] > -1.0f)
    {
        boneTransform += skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[0]].finalTransform * inBoneWeights[0];
    }
    if (skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[1]].finalTransform != mat4(0.0) && inBoneWeights[1] > -1.0f)
    {
        boneTransform += skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[1]].finalTransform * inBoneWeights[1];
    }
    if (skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[2]].finalTransform != mat4(0.0) && inBoneWeights[2] > -1.0f)
    {
        boneTransform += skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[2]].finalTransform * inBoneWeights[2];
    }
    if (skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[3]].finalTransform != mat4(0.0) && inBoneWeights[3] > -1.0f)
    {
        boneTransform += skeletalAnimationBuffer.animatedBones[animIndex + inBoneIDs[3]].finalTransform * inBoneWeights[3];
    }

    if (boneTransform == mat4(0.0))
    {
        boneTransform = mat4(1.0);
    }

    return boneTransform;
}

void main() 
{
    ObjectData object = objectBuffer.objects[gl_InstanceIndex];

    vec4 animatedPos = vec4(1.0f);
    mat4 boneTransform = mat4(1.0f);

    if (object.skeletalAnimationIndex != 4294967295)
    {
        boneTransform = ApplyAnimation(object.skeletalAnimationIndex);
        animatedPos = boneTransform * vec4(inPos, 1.0);
    }
    else
    {
        animatedPos = vec4(inPos, 1.0);
    }

    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = 1 + object.outlineWidthPercentage * 0.01;
    scaleMatrix[1][1] = 1 + object.outlineWidthPercentage * 0.01;
    scaleMatrix[2][2] = 1 + object.outlineWidthPercentage * 0.01;

    animatedPos = scaleMatrix * animatedPos;
    gl_Position = environmentData.cameraViewProjection * object.model * animatedPos;
}
