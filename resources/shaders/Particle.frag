#version 450

layout(push_constant) uniform PER_OBJECT
{
    // MVP matrices
    mat4 model;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
} pc;

layout (set = 0, binding = 2) uniform sampler2D textures[1000];

layout (location = 0) in vec3 inFragWorldPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

vec4 MaterialColor(int textureIndex)
{
   return vec4(texture(textures[textureIndex], inUV));
}

void main() 
{
    vec4 fragColor = MaterialColor(pc.baseColorIndex);

    if(fragColor.a < 0.03)
        discard;
    outFragColor = fragColor;
}