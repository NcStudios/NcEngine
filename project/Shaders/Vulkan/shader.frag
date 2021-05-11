#version 450

layout (location = 0) in vec4 inScreenPos;
layout (location = 1) in vec4 inLocalPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (set = 1, binding = 0) uniform sampler2D baseColor;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    vec3 color = texture(baseColor, inUV).xyz;
	outFragColor = vec4(color, 1.0);
}