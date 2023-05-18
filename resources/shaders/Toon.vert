#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;
    
    // Textures
    vec4 unused;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec3 outFragPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec2 outUV;
layout (location = 3) out int  outObjectInstance;

void main() 
{
    ObjectData object = objectBuffer.objects[gl_BaseInstance];

    gl_Position = object.viewProjection * object.model * vec4(inPos, 1.0);
    outFragPosition = vec3(object.model * vec4(inPos, 1.0));
    outNormal = mat3(object.model) * inNormal;
    outUV = inUV;
    outObjectInstance = gl_BaseInstance;
}