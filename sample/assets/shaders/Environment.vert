#version 450

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    uint unused1;
    uint unused2;
    uint unused3;
    uint unused4;

    uint skeletalAnimationIndex;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[10000];
} objectBuffer;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in vec4 inBoneWeights;
layout (location = 6) in ivec4 inBoneIDs;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() 
{
    ObjectData object = objectBuffer.objects[gl_InstanceIndex];
    outUVW = inPos;
    gl_Position = object.viewProjection * object.model * vec4(inPos, 1.0);
}