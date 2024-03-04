// Credit to: https://learnopengl.com/Getting-started/OpenGL

#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

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

struct ObjectData
{
    // N MVP matrices
    mat4 model;
    mat4 modelView;
    mat4 viewProjection;

    // Textures
    uint baseColorIndex;
    uint normalIndex;
    uint roughnessIndex;
    uint metallicIndex;

    uint skeletalAnimationIndex;
};

layout(std140, set=0, binding=0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;


layout (std140, set=0, binding=1) readonly buffer PointLightsArray
{
    PointLight lights[];
} pointLights;

layout (set = 1, binding = 2) uniform sampler2D textures[];
layout (set = 0, binding = 3) uniform sampler2D shadowMaps[];
layout (set = 1, binding = 4) uniform samplerCube cubeMaps[];

layout (set = 0, binding = 5) uniform EnvironmentDataBuffer
{
    vec4 cameraWorldPosition;
    int skyboxCubemapIndex;
} environmentData;

layout (location = 0) in vec3 inViewPosition;
layout (location = 1) in vec3 inFragPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inUV;
layout (location = 4) in mat3 inTBN;
layout (location = 7) in flat int inObjectInstance;
layout (location = 8) in vec3 inUVW;

layout (location = 0) out vec4 outFragColor;

vec3 MaterialColor(uint textureIndex)
{
   return vec3(texture(textures[textureIndex], inUV));
}

vec3 SkyboxColor(int cubeMapIndex, vec3 angleVector)
{
    return vec3(texture(cubeMaps[cubeMapIndex], angleVector));
}

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, int index)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMaps[index], projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps[index], 0);

    for(int x = 0; x <=1; ++x)
    {
        for(int y = 0; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;  

    if (projCoords.z > 1.0 || projCoords.z < 0)
    {
        shadow = 0.0;
    }

    return shadow;
}

vec3 CalculatePointLight(int lightIndex, vec3 N, vec3 V, vec3 F0, vec3 baseColor, float roughness, float metallic, vec4 lightViewPos)
{
    PointLight light = pointLights.lights[lightIndex];

    // Per light radiance
    vec3 L = normalize(light.lightPos - inFragPosition);
    vec3 H = normalize(V + L);
    float distance = length(light.lightPos - inFragPosition);
    float attenuation = (1.0 / (distance * distance)) * (light.specularIntensity * 0.8);
    vec3 radiance = light.diffuseColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H,V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4 * max(dot(N,V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    vec3 colorTotal = (kD * baseColor / PI + specular) * radiance * NdotL;

    float shadow = 0.0;

    // Shadow
    if (light.castShadows == 1)
    {
        shadow = ShadowCalculation(lightViewPos, lightIndex);
    }

    return (1.0 - shadow) * colorTotal;
}

const mat4 biasMat = mat4( 
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 
);

void main() 
{
    vec3 baseColor = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex);
    vec3 normalColor = MaterialColor(objectBuffer.objects[inObjectInstance].normalIndex);
    vec3 roughnessColor = MaterialColor(objectBuffer.objects[inObjectInstance].roughnessIndex);
    vec3 metallicColor = MaterialColor(objectBuffer.objects[inObjectInstance].metallicIndex);

    vec3 normal = 2.0 * normalColor - 1.0;
    vec3 N = (normalize(inTBN * normal));
    vec3 V = normalize(environmentData.cameraWorldPosition.rgb - inFragPosition);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, baseColor, metallicColor.r);

    vec3 result = vec3(0.0);
    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        vec4 lightViewPos = vec4(1.0f);
        if (pointLights.lights[i].castShadows == 1)
        {
            lightViewPos = biasMat * pointLights.lights[i].lightViewProj * vec4(inFragPosition, 1.0);
        }

        result += CalculatePointLight(i, N, V, F0, baseColor, roughnessColor.r, metallicColor.r, lightViewPos) + (pointLights.lights[i].ambientColor * 0.015);
    }

   if (environmentData.skyboxCubemapIndex != 4294967295)
    {
        // Environment reflection
        vec3 I = normalize(inFragPosition - environmentData.cameraWorldPosition.rgb);
        vec3 reflected = reflect(I, N);
        vec3 environmentReflectionColor = SkyboxColor(environmentData.skyboxCubemapIndex, reflected);

        result += (F0 * (1-roughnessColor.r) * environmentReflectionColor) / 2;
    }

    outFragColor = vec4(result, 1.0);
}