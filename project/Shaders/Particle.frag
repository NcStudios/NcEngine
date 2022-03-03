#version 460
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
} pc;

layout (set = 0, binding = 2) uniform sampler2D textures[];

layout (location = 0) in vec3 inFragWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in mat3 inTBN;

layout (location = 0) out vec4 outFragColor;

vec4 MaterialColor(int textureIndex)
{
   return vec4(texture(textures[textureIndex], inUV));
}

void main() 
{
    outFragColor = MaterialColor(pc.baseColorIndex);
}