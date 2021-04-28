#version 450

layout (location = 0) in vec4 inScreenPos;
layout (location = 1) in vec4 inLocalPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = vec4(0.5, 0.5, 0.5, 1.0);
}