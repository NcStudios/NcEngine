#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    layout(offset = 128) vec4 color;
} pc;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = pc.color;
}
