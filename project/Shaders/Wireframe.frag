#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // MVP matrices
    mat4 model;
    mat4 viewProjection;
} pc;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = vec4(1.0, 0.0, 0.0, 1.0);
}