#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 normal;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;

    int isInitialized;
};

layout(std140, set=2, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout (location = 0) out vec3 outFragWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out mat3 outTBN;
layout (location = 6) out int  outObjectInstance;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    outNormal = vec3(objectBuffer.objects[gl_BaseInstance].normal * vec4(inNormal, 1.0));
    outUV = inUV;
    outFragWorldPos = vec3(objectBuffer.objects[gl_BaseInstance].model * vec4(inPos, 1.0));

    vec3 T = normalize(vec3(objectBuffer.objects[gl_BaseInstance].normal * vec4(inTangent, 0.0)));
    vec3 B = normalize(vec3(objectBuffer.objects[gl_BaseInstance].normal * vec4(inBitangent, 0.0)));
    vec3 N = normalize(vec3(objectBuffer.objects[gl_BaseInstance].normal * vec4(inNormal, 0.0)));
    outTBN = mat3(T, B, N);

    outObjectInstance = gl_BaseInstance;

    gl_Position = objectBuffer.objects[gl_BaseInstance].viewProjection * objectBuffer.objects[gl_BaseInstance].model * vec4(inPos, 1.0);
}