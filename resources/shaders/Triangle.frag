#version 140

layout (location = 0) in vec3 InColor
layout (location = 0) out vec4 OutColor

void main()
{
    OutColor = float4(InColor.rgb, 1.0);
}