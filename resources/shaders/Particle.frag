#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ParticleData
{
    mat4 model;
    uint textureIndex;
};

layout(std140, set=0, binding=7) readonly buffer ParticleBuffer
{
    ParticleData particles[];
} particleBuffer;

layout(push_constant) uniform PER_OBJECT
{
    mat4 viewProjection;
} pc;

layout (set = 1, binding = 2) uniform sampler2D textures[];

layout (location = 0) in vec3 inFragWorldPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in flat int inInstanceIndex;

layout (location = 0) out vec4 outFragColor;

vec4 MaterialColor(uint textureIndex)
{
   return vec4(texture(textures[textureIndex], inUV));
}

void main()
{
    outFragColor = MaterialColor(particleBuffer.particles[inInstanceIndex].textureIndex);
}
