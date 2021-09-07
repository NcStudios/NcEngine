#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

layout(push_constant) uniform PER_OBJECT
{
    // MVP matrix for point light
    mat4 depthMVP;
} pc;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main()
{
	gl_Position = pc.depthMVP * vec4(inPos, 1.0);
}