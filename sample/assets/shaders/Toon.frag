#version 450

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    uint baseColorIndex;
    uint overlayIndex;
    uint hatchingIndex;
    uint hatchingTiling;

    uint skeletalAnimationIndex;
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
    ObjectData objects[100000];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[20];
} pointLights;

layout (set = 0, binding = 2) uniform sampler2D textures[1000];
layout (set = 0, binding = 4) uniform samplerCube cubeMaps[1000];
layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    vec3 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

layout (location = 0) in vec3 inFragPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in flat int inObjectInstance;

layout (location = 0) out vec4 outFragColor;

vec4 MaterialColor(uint textureIndex, uint scale)
{
    return vec4(texture(textures[textureIndex], inUV * scale));
}

vec4 SkyboxColor(int cubeMapIndex, vec3 angleVector)
{
    return vec4(texture(cubeMaps[cubeMapIndex], angleVector));
}

void main() 
{
    vec4 result = vec4(0.0);
    float alpha = 1.0f;

    // Material data
    vec4 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1u);
    alpha = baseColor.a;
    float hatchingTexture = MaterialColor(objectBuffer.objects[inObjectInstance].hatchingIndex, objectBuffer.objects[inObjectInstance].hatchingTiling).x;

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0) continue;

        // Light data
        PointLight light = pointLights.lights[i];
        vec3 lightDir = normalize(light.lightPos - inFragPosition);
        float lightIntensity = dot(lightDir, normalize(inNormal));
        vec4 lightColor = vec4(light.diffuseColor, 1.0);
        vec4 lightAmbient = vec4(light.ambientColor, 1.0);

        // Cel shading levels
        float highlightLevel = 0.85f;
        float hatchingLevel = 0.4f;
        float darkestLevel = 0.0f;
        float blurAmount = 0.05f;
        vec4 pixelColor = baseColor;

        if (lightIntensity <= darkestLevel + blurAmount)
        {
            pixelColor = pixelColor * lightColor * mix(darkestLevel, hatchingTexture, (lightIntensity - darkestLevel) * 1/(blurAmount));
        }
        else if (lightIntensity <= hatchingLevel)
        {
            pixelColor *= lightColor * hatchingTexture;
        }
        else if (lightIntensity <= hatchingLevel + blurAmount)
        {
            pixelColor = pixelColor * lightColor * mix(hatchingTexture, 1.0f, (lightIntensity - hatchingLevel) * 1/(blurAmount));
        }
        else if (lightIntensity <= highlightLevel)
        {
            pixelColor *= lightColor;
        }
        else if (lightIntensity <= highlightLevel + blurAmount)
        {
            pixelColor *=  mix(lightColor, (lightColor + lightAmbient), (lightIntensity - highlightLevel) * 1/(blurAmount));
        }
        else pixelColor *= lightColor + lightAmbient;
        result += max(vec4(0.0f), pixelColor);
    }

    if (environmentData.skyboxCubemapIndex > -1)
    {
        // Environment reflection
        vec3 I = normalize(inFragPosition - environmentData.cameraWorldPosition);
        vec3 surfaceNormal = normalize(inNormal);
        vec3 reflected = reflect(I, surfaceNormal);
        vec4 environmentReflectionColor = SkyboxColor(environmentData.skyboxCubemapIndex, reflected);
        result = mix(result, result + environmentReflectionColor, 0.01f);
    }


    // Overlay
    result = mix(result, result * MaterialColor(objectBuffer.objects[inObjectInstance].overlayIndex, objectBuffer.objects[inObjectInstance].hatchingTiling/2), 0.9f);

    outFragColor = vec4(result.r, result.g, result.b, alpha);
}
