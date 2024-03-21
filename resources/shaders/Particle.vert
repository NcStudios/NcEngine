#version 450
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

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout (location = 0) out vec3 outFragWorldPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out int outInstanceIndex;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    ParticleData particle = particleBuffer.particles[gl_InstanceIndex];
    outUV = inUV;
    outFragWorldPos = vec3(particle.model * vec4(inPos, 1.0));
    outInstanceIndex = gl_InstanceIndex;

    gl_Position = pc.viewProjection * particle.model * vec4(inPos, 1.0);
}
