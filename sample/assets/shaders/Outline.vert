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
    int baseColorIndex;
    int overlayIndex;
    int lightShadingIndex;
    int heavyShadingIndex;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

void main() 
{
    ObjectData object = objectBuffer.objects[gl_BaseInstance];
    gl_Position = object.viewProjection * object.model * vec4(inPos + inNormal * 0.005, 1.0);
}