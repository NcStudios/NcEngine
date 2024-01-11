#version 450

layout(push_constant) uniform PER_OBJECT
{
    // MVP matrices
    mat4 model;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
} pc;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;
layout (location = 5) in vec4 inBoneWeights;
layout (location = 6) in ivec4 inBoneIDs;

layout (location = 0) out vec3 outFragWorldPos;
layout (location = 1) out vec2 outUV;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    outUV = inUV;
    outFragWorldPos = vec3(pc.model * vec4(inPos, 1.0));

    gl_Position = pc.viewProjection * pc.model * vec4(inPos, 1.0);
}