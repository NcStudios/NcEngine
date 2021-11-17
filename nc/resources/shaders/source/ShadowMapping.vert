#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec3 inPos;

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;
    int metallicIndex;
};

layout(push_constant) uniform PER_OBJECT
{
    // VP matrix for point light
    mat4 lightViewProj;
} pc;

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

void main()
{
    // Calculate the vertex's position in the view space of the point light
    // Take the world space position of the vertex (vertex position * model matrix for that vertex's object), then multiply by the view projection of the light.
    vec4 worldPosOfVertex = objectBuffer.objects[gl_BaseInstance].model * vec4(inPos, 1.0);
	gl_Position = pc.lightViewProj * worldPosOfVertex;
}