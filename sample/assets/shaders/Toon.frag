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

const float Epsilon = 1e-10;

vec3 RGBtoHSV(in vec3 RGB)
{
    vec4  P   = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0/3.0) : vec4(RGB.gb, 0.0, -1.0/3.0);
    vec4  Q   = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
    float C   = Q.x - min(Q.w, Q.y);
    float H   = abs((Q.w - Q.y) / (6.0 * C + Epsilon) + Q.z);
    vec3  HCV = vec3(H, C, Q.x);
    float S   = HCV.y / (HCV.z + Epsilon);
    return vec3(HCV.x, S, HCV.z);
}

vec3 HSVtoRGB(in vec3 HSV)
{
    float H   = HSV.x;
    float R   = abs(H * 6.0 - 3.0) - 1.0;
    float G   = 2.0 - abs(H * 6.0 - 2.0);
    float B   = 2.0 - abs(H * 6.0 - 4.0);
    vec3  RGB = clamp( vec3(R,G,B), 0.0, 1.0 );
    return ((RGB - 1.0) * HSV.y + 1.0) * HSV.z;
}

void main() 
{
    vec4 result = vec4(0.0);
    float alpha = 1.0f;

    // Material data
    vec4 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1u);
    alpha = baseColor.a;
    float hatchingTexture = MaterialColor(objectBuffer.objects[inObjectInstance].hatchingIndex, objectBuffer.objects[inObjectInstance].hatchingTiling).x;
    if (hatchingTexture < 0.6)
    {
        hatchingTexture = 0.0f;
    }

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0) continue;

        // Light data
        PointLight light = pointLights.lights[i];
        vec3 lightDir = normalize(light.lightPos - inFragPosition);
        float lightIntensity = dot(lightDir, normalize(inNormal));
        float distanceToLight = length(light.lightPos - inFragPosition);
        float lightRadius = light.specularIntensity;
        float lightAttenuated = max(0.0, 1.0 - distanceToLight / lightRadius) * dot(lightDir, normalize(inNormal));

        vec4 lightColor = vec4(light.diffuseColor, 1.0);
        vec4 lightAmbient = vec4(light.ambientColor, 1.0);

        // Cel shading levels
        float highlightLevel = 0.83f;
        float hatchingLevel = 0.5f;
        float darkestLevel = 0.0f;
        float blurAmount = 0.0025f;
        vec4 pixelColor = baseColor;

        if (lightIntensity <= darkestLevel + blurAmount)
        {
            pixelColor = pixelColor * lightColor * mix(darkestLevel, hatchingTexture, (lightIntensity - darkestLevel) * 1/(blurAmount));
        }
        else if (lightIntensity <= hatchingLevel)
        {
            pixelColor *= lightColor * hatchingTexture;
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
        float snappedLight = round(lightAttenuated / 0.3) * 0.3;
        result += max(vec4(0.0f), pixelColor) * mix(snappedLight, lightAttenuated, 0.8);
    }

    // Overlay
    result = mix(result, result * MaterialColor(objectBuffer.objects[inObjectInstance].overlayIndex, objectBuffer.objects[inObjectInstance].hatchingTiling/2), 0.9f);

    vec3 col_hsv = RGBtoHSV(vec3(result.r, result.g, result.b));
    col_hsv.y *= (0.6 * 2.0);
    vec3 col_rgb = HSVtoRGB(col_hsv.rgb);
    outFragColor = vec4(col_rgb.r, col_rgb.g, col_rgb.b, alpha);
}
