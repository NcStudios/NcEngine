#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}