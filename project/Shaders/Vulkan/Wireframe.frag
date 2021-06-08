#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(push_constant) uniform PER_OBJECT
{
    // N MVP matrices
    mat4 normal; // Transforms the vertex data normals into world space
    mat4 model;
    mat4 viewProjection;

    // Camera world position
    vec3 cameraPos;

    // Textures
    int baseColorIndex;
    int normalIndex;
    int roughnessIndex;
} pc;

struct PointLight
{
    vec4 lightPos;
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
    bool isInitialized;
};

layout (location = 0) out vec4 outFragColor;

void main() 
{
    outFragColor = vec4(1.0, 0.0, 0.0, 1.0);
}