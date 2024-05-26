#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    mat4 cameraViewProjection;
    vec4 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

layout(push_constant) uniform PER_OBJECT
{
    mat4 model;
} pc;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() 
{
    gl_Position = environmentData.cameraViewProjection * pc.model * vec4(inPos, 1.0);
}
