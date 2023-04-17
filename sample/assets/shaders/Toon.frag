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

struct PointLight
{
    mat4 lightViewProj;
    vec3 lightPos;
    int castShadows;
    vec3 ambientColor;
    float attLin;
    vec3 diffuseColor;
    float attQuad;
    float specularIntensity;
    int isInitialized;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (set = 0, binding = 2) uniform sampler2D textures[];

layout (location = 0) in vec3 inFragPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in flat int inObjectInstance;
layout (location = 4) in vec3 inStencilPosition;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(int textureIndex, int scale)
{
   return vec3(texture(textures[textureIndex], inUV * scale));
}

void main() 
{
    vec3 result = vec3(0.0);

//    for (int i = 0; i < pointLights.lights.length(); i++)
//    {
        PointLight light = pointLights.lights[0];
        vec3 lightDir = normalize(light.lightPos - inFragPosition);
        float intensity = dot(lightDir, normalize(inNormal));
        vec3 color = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1);
        float normalIntensity = MaterialColor(objectBuffer.objects[inObjectInstance].heavyShadingIndex, 8).x;
        vec3 lightColor = light.diffuseColor;
        vec3 sampledColor;
        vec3 baseColor = color;
        float normalDetail = 1.0f;
        if (normalIntensity > 0.5f)
        {
            normalDetail = 0.0f;
        }

        float hatchingMin = 0.2f;
        float hatchingMax = 0.3f;
        float pureBlackThreshold = 0.05f;
        float firstLevelThreshold = 0.2f;
        float secondLevelThreshold = 0.25f;
        float thirdLevelThreshold = 0.65f;
        float midLevel = 0.7f;
        if (intensity <= pureBlackThreshold)
        {
            color = color * lightColor * mix(0.0f, normalIntensity, (intensity * 1/pureBlackThreshold)) * midLevel;
        }
        else if (intensity <= firstLevelThreshold)
        {
            color *= lightColor * normalIntensity * midLevel;
        }
        else if (intensity <= secondLevelThreshold)
        {
            color = color * lightColor * mix(normalIntensity, 1.0f, (intensity - firstLevelThreshold) * 1/(secondLevelThreshold - firstLevelThreshold)) * midLevel;
        }
        else if (intensity <= thirdLevelThreshold)
        {
            color *= lightColor * midLevel;
        }
        else
        {
            color *= lightColor;
        }
        result += color;
//    }
    outFragColor = vec4(result, 1.0f);
}
