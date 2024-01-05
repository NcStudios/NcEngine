#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    uint baseColorIndex;
    uint normalIndex;
    uint roughnessIndex;
    uint metallicIndex;

    uint skeletalAnimationIndex;
};

struct PointLight
{
    mat4 lightViewProj;
    vec3 lightPos;
    int castShadows;
    vec3 ambientColor;
    float attLin;
    vec3 diffuseColor;
    float attQuad;
    float diffuseIntensity;
    int isInitialized;
};

struct SkeletalAnimationData
{
    mat4 finalTransform;
};

layout(std140, set=0, binding=0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (std140, set=0, binding=6) readonly buffer AnimatedBoneTransforms
{
    SkeletalAnimationData animatedBones[];
} skeletalAnimationBuffer;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in vec4 inBoneWeights;
layout (location = 6) in ivec4 inBoneIDs;

layout (location = 0) out vec3 outViewPosition;
layout (location = 1) out vec3 outFragPosition;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec2 outUV;
layout (location = 4) out mat3 outTBN;
layout (location = 7) out int  outObjectInstance;
layout (location = 8) out vec3 outUVW;


out gl_PerVertex {
    vec4 gl_Position;
};

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
    ObjectData object = objectBuffer.objects[gl_BaseInstance];
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

    outNormal = mat3(object.model) * inNormal;
    vec3 N = normalize(vec3(object.model * vec4(inNormal,    0.0)));
    vec3 T = normalize(vec3(object.model * vec4(inTangent,   0.0)));
    vec3 B = normalize(cross(N, T));
    outTBN = mat3(T, B, N);
    outUV = inUV;
    outObjectInstance = gl_BaseInstance;
    outUVW = vec3(animatedPos);
 
    outFragPosition = vec3(object.model * animatedPos);
    gl_Position = object.viewProjection * object.model * animatedPos;
}