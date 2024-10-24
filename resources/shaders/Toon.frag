#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    mat4 model;

    // Textures
    uint baseColorIndex;
    uint outlineWidth;
    uint hatchingIndex;
    uint hatchingTiling;

    uint skeletalAnimationIndex;
};

struct PointLight
{
    mat4 viewProjection;
    vec3 position;
    int castsShadows;
    vec3 ambientColor;
    int isInitialized;
    vec3 diffuseColor;
    float radius;
};

struct SpotLight
{
    mat4 viewProjection;
    vec3 position;
    int castsShadows;
    vec3 color;
    int isInitialized;
    vec3 direction;
    float innerAngle;
    float outerAngle;
    float radius;
};

layout(std140, set=0, binding = 0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (set = 2, binding = 3) uniform sampler2D shadowMaps[];

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    mat4 cameraViewProjection;
    vec4 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

layout (std140, set=0, binding=8) readonly buffer SpotLightsArray
{
    SpotLight lights[];
} spotLights;

layout (set = 1, binding = 2) uniform sampler2D textures[];
layout (set = 1, binding = 4) uniform samplerCube cubeMaps[];

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

float ShadowCalculation(vec4 fragPosLightSpace, int index)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[index], 0);
    int sampleRadius = 2;

    for(int y = -sampleRadius; y <= sampleRadius; y++)
    {
        for (int x = - sampleRadius; x <= sampleRadius; x++)
        {
            // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            float closestDepth = texture(shadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r; 
            if (currentDepth > closestDepth)
            {
                shadow += 1.0f;
            }
        }
    }
    
    shadow /= pow((sampleRadius * 2 + 1), 2);

    if (projCoords.z > 1.0 || projCoords.z < 0)
    {
        shadow = 0.0;
    }

    return shadow;
}

const mat4 biasMat = mat4( 
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 
);

float PointLightRadiance(PointLight light, vec3 fragPosition, vec3 normal, int shadowIndex)
{
    vec3 L = normalize(light.position - fragPosition); // The vector from the light to the fragment.
    float NDotL = max(dot(normal, L), 0.0f); // The intensity of the light
    float distance = length(light.position - fragPosition);
    float attenuation = clamp(1/(pow(distance, 4.0f)), 0.0, 1.0) * pow(light.radius, 3);
    float intensity = NDotL * attenuation;

    if (light.castsShadows == 1)
    {
        intensity *= (1.0f - ShadowCalculation(biasMat * light.viewProjection * vec4(fragPosition, 1.0), shadowIndex));
    }

    return intensity;
}

float SpotLightRadiance(SpotLight light, vec3 fragPosition, int shadowIndex)
{
    // Calculate the radiance for the light source
    vec3 L = normalize(light.position - fragPosition); // The vector from the light to the fragment.
    float theta = dot(L, normalize(-light.direction));
    float distance = length(light.position - fragPosition);
    float attenuation = clamp(1/(pow(distance, 4.0f)), 0.0, 1.0);
    float falloff = clamp((theta - light.outerAngle) / (light.innerAngle - light.outerAngle), 0.0f, 1.0f);
    float intensity = falloff * attenuation * pow(light.radius, 3);

    if (light.castsShadows == 1)
    {
        intensity *= (1.0f - ShadowCalculation(biasMat * light.viewProjection * vec4(fragPosition, 1.0), shadowIndex));
    }

    return intensity;
}

void main() 
{
    // Process inputs
    vec4 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex, 1u);
    float alpha = baseColor.a;
    float hatchingTexture = MaterialColor(objectBuffer.objects[inObjectInstance].hatchingIndex, objectBuffer.objects[inObjectInstance].hatchingTiling).x;
    vec3 N = normalize(inNormal);

    vec4 result = vec4(0.0f);
    vec4 lightColor = vec4(0.0f);
    float lightIntensity = 0.0f;

    int activePointLights = 0;
    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        PointLight light = pointLights.lights[i];
        if (light.isInitialized == 0) continue;
        activePointLights++;

        // Light data
        lightIntensity += PointLightRadiance(light, inFragPosition, N, i);
        lightColor += vec4(light.diffuseColor, 1.0f);
    }

    for (int i = 0; i < spotLights.lights.length(); i++)
    {
        SpotLight light = spotLights.lights[i];
        if (light.isInitialized == 0) continue;

        // Light data
        lightIntensity += SpotLightRadiance(light, inFragPosition, i + activePointLights);
        lightColor += vec4(light.color, 1.0f);
    }

    if (lightIntensity > 0.82)
    {
        result += lightColor;
    }
    else if (lightIntensity > 0.5)
    {
        float t = smoothstep(0.80, 0.82, lightIntensity);
        result += mix(lightColor * 0.6f, lightColor, t);
    }
    else if (lightIntensity > 0.25)
    {
        float t = smoothstep(0.48, 0.5, lightIntensity);
        result += mix(lightColor * 0.3f, lightColor * 0.6f, t);
    }
    else
    {
        float t = smoothstep(0.23, 0.25, lightIntensity);
        result += mix(lightColor * 0.15f, lightColor * 0.3f, t);
    }

    result += mix(vec4(0.0f), lightColor * lightIntensity, 0.2f);

    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        PointLight light = pointLights.lights[i];
        if (light.isInitialized == 0) continue;
        result += (vec4(light.ambientColor, 1.0f) * 0.4f);
    }

    result *= baseColor;

    vec3 col_hsv = RGBtoHSV(vec3(result.r, result.g, result.b));
    col_hsv.y *= (0.45 * 2.0);
    vec3 col_rgb = HSVtoRGB(col_hsv.rgb);
    outFragColor = vec4(col_rgb.r, col_rgb.g, col_rgb.b, alpha);
}
