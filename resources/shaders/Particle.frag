#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // MVP matrices
    mat4 model;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
} pc;

layout (set = 1, binding = 2) uniform sampler2D textures[];

layout (location = 0) in vec3 inFragWorldPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

vec4 MaterialColor(int textureIndex)
{
   return vec4(texture(textures[textureIndex], inUV));
}

void main() 
{
    outFragColor = MaterialColor(pc.baseColorIndex);
}