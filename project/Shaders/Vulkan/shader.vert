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
} pushConstants;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outScreenPos;
layout (location = 1) out vec4 outLocalPos;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec2 outUV;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() 
{
    outScreenPos =  vec4(inPos, 1.0) * pushConstants.modelView;
    outLocalPos = vec4(inPos, 1.0);
	outNormal = inNormal;
	outUV = inUV;
	gl_Position = outScreenPos;
}