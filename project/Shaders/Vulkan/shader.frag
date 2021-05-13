#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    mat4 model;
    mat4 modelView;
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;
} pc;

layout (set = 0, binding = 0) uniform sampler smplr;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (location = 0) in vec4 inScreenPos;
layout (location = 1) in vec4 inLocalPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = texture(sampler2D(textures[pc.baseColorIndex], smplr), inUV);
}