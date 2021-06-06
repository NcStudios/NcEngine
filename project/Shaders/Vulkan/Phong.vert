#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // N MVP matrices
    mat4 normal; // Transforms the vertex data normals into world space
    mat4 model;
    mat4 viewProjection;

    // Camera world position
    vec3 cameraPos;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;
} pc;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout (location = 0) out vec3 outFragWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out mat3 outTBN;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    outNormal = vec3(pc.normal * vec4(inNormal, 1.0));
    outUV = inUV;
    outFragWorldPos = vec3(pc.model * vec4(inPos, 1.0));

    vec3 T = normalize(vec3(pc.normal * vec4(inTangent, 0.0)));
    vec3 B = normalize(vec3(pc.normal * vec4(inBitangent, 0.0)));
    vec3 N = normalize(vec3(pc.normal * vec4(inNormal, 0.0)));
    outTBN = mat3(T, B, N);

    gl_Position = pc.viewProjection * pc.model * vec4(inPos, 1.0);
}