#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec3 inUVW;
layout (location = 0) out vec4 outFragColor;

// layout (set = 0, binding = 4) uniform samplerCube cubeMaps[];

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    vec3 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

// vec3 SkyboxColor(int cubeMapIndex, vec3 angleVector)
// {
//     return vec3(texture(cubeMaps[cubeMapIndex], angleVector));
// }

void main() 
{
    // outFragColor = vec4(SkyboxColor(environmentData.skyboxCubemapIndex, inUVW), 1.0);
    outFragColor = vec4(1.0,1.0,1.0, 1.0);
}