#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec3 inPos;
layout (location = 5) in vec4 inBoneWeights;
layout (location = 6) in ivec4 inBoneIDs;

layout (location = 0) out vec4 outPos;
layout (location = 1) out vec3 outLightPos;

struct ObjectData
{
    mat4 model;

    // Textures
    uint unused1;
    uint unused2;
    uint unused3;
    uint unused4;

    uint skeletalAnimationIndex;
};

struct PointLight
{
    mat4 viewProjection;
    vec3 position;
    int castsShadows;
    vec3 ambientColor;
    int isInitialized;
    vec3 diffuseColor;
    float radius;
};

struct SkeletalAnimationData
{
    mat4 finalTransform;
};

layout(push_constant) uniform PER_OBJECT
{
    mat4 lightViewProj;
    uint lightIndex;
} pc;

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
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

mat4 translate(vec3 translation) {
    return mat4(1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                translation.x, translation.y, translation.z, 1.0);
}

void main()
{
    // Calculate the vertex's position in the view space of the point light
    // Take the world space position of the vertex (vertex position * model matrix for that vertex's object), then multiply by the view projection of the light.
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

    vec4 worldPosOfVertex = objectBuffer.objects[gl_InstanceIndex].model * animatedPos;
    gl_Position = pc.lightViewProj * worldPosOfVertex;
    outPos = worldPosOfVertex;
    outLightPos = pointLights.lights[pc.lightIndex].position;
}
