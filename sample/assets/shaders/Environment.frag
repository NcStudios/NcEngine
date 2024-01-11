#version 450

layout (location = 0) in vec3 inUVW;
layout (location = 0) out vec4 outFragColor;

layout (set = 0, binding = 4) uniform samplerCube cubeMaps[1000];

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    vec3 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

vec3 SkyboxColor(int cubeMapIndex, vec3 angleVector)
{
    return vec3(texture(cubeMaps[cubeMapIndex], angleVector));
}

void main() 
{
    outFragColor = vec4(SkyboxColor(environmentData.skyboxCubemapIndex, inUVW), 1.0);
}