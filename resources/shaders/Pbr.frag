/** PBR Shader Using Cook-Torrance BRDF 

TERM             | EXPLANATION                                                    | NOTES
----------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------
Pixel Color      | Infinite Sum of:  BRDF(P, Wi, Wo) * L(P, Wi) * (N ⋅ Wi) * dW   |  
BRDF             | (Kd * diffuseFunction) + (Ks * specularFunction)               | Kd + Ks must = 1 due to conservation of energy.
Kd               | 1 - Ks                                                         |  
diffuseFunction  | (color / π) * (L ⋅ N)                                           | Lambertian
Ks               | F₀ + (1 - F₀)(1 - (V ⋅ H))^5                                    | Fresnel function
F₀               | Reflectivity                                                    |     
V                | Vector from camera to fragment                                  |    
H                | Half vector                                                     |   
specularFunction | (D * G * F) / 4(V ⋅ N) * (L ⋅ N)                                 | Cook-Torrance
D                | Normal Distribution Function                                    | NDF - The distribution of microfacet normals compared to the half vector, aka, the roughness
G                | Geometry Shadowing Function                                     | GSF - The attenuation of light due to microfacet self-shadowing
F                | F₀ + (1 - F₀)(1 - (V ⋅ H))^5                                    | Fresnel function, but it's already in our specularK so we don't need it twice.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
**/

#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

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

layout (location = 0) in vec3 inFragPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in mat3 inTBN;
layout (location = 5) in flat int  inObjectInstance;

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

// D: Normal Distribution Function (Trowbridge-Reitz)
float D(float roughness, float NdotH)
{
    float roughnessSqr = roughness*roughness;
    float Distribution = NdotH*NdotH * (roughnessSqr-1.0) + 1.0;
    return roughnessSqr / (PI * Distribution*Distribution);
}

// G: Geometry Shadowing Function (Implicit)
float G(float NDotL, float NDotV, float roughness)
{
    return NDotL * NDotV;
}

// F: Fresnel-Schlick Function
vec3 F(vec3 F0, vec3 V, vec3 H)
{
    return F0 + (vec3(1.0f) - F0) * pow(1 - max(dot(V, H), 0.0f), 5.0f);
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

vec3 CalculatePointLight(int index, PointLight light, vec3 fragPosition, vec3 baseColor, float roughness, float metallic, vec3 emissivityColor, vec3 N, vec3 V, vec3 F0)
{
    vec3 L = normalize(light.position - fragPosition); // The vector from the light to the fragment. Note, for directional lights, L would just be the position.
    vec3 H = normalize(V + L); // The half-vector between the vector from the light to the fragment and the vector from the camera to the fragment.

    float NDotH = max(dot(N, H), 0.0f);
    float NDotL = max(dot(N, L), 0.0f);
    float NDotV = max(dot(N, V), 0.0f);
    float VDotN = max(dot(V, N), 0.0f);
    float LDotN = max(dot(L, N), 0.0f);

    vec3 Ks = F(F0, V, H); // The specular component
    vec3 Kd = (vec3(1.0f) - Ks) * (1.0f - metallic); // The diffuse component

    vec3 diffuseBrdf = baseColor / PI; // Lambertian BRDF for diffuse lighting

    vec3 specularBrdfNumerator = D(roughness, NDotH) * G(NDotL, NDotV, roughness) * F(F0, V, H); // The numerator of the Cook-Torrance BRDF (Normal Distribution * Geometric Shadowing * Fresnel)
    float specularBrdfDenominator = max(4.0f * VDotN * LDotN, 0.000001f);
    vec3 specularBrdf = specularBrdfNumerator / specularBrdfDenominator;

    vec3 brdf = Kd * diffuseBrdf + specularBrdf; // No need to multiply by Ks because we already multiplied our numerator by F in the specularBrdfNumerator

    // Calculate the radiance for the light source
    float distance = length(light.position - fragPosition);
    float attenuation = clamp(1/(pow(distance, 4.0f)), 0.0, 1.0);
    vec3 radiance = light.diffuseColor * attenuation * pow(light.radius, 3);

    vec3 perLightResult = emissivityColor + brdf * radiance * LDotN;

    float shadow = 0.0;

    // Shadow
    if (light.castsShadows == 1)
    {
        shadow = ShadowCalculation(biasMat * light.viewProjection * vec4(fragPosition, 1.0), index);
    }

    perLightResult *= (1.0 - shadow);
    return perLightResult;
}

void main() 
{
    // Import material textures
    vec3 baseColor      = MaterialColor(objectBuffer.objects[inObjectInstance].baseColorIndex);
    vec3 normalColor    = MaterialColor(objectBuffer.objects[inObjectInstance].normalIndex);
    vec3 roughnessColor = MaterialColor(objectBuffer.objects[inObjectInstance].roughnessIndex);
    vec3 metallicColor  = MaterialColor(objectBuffer.objects[inObjectInstance].metallicIndex);
    vec3 emissivityColor = vec3(0.0f, 0.0f, 0.0f);
    vec3 ambientOcclusionColor = vec3(1.0f, 1.0f, 1.0f);

    float roughness = roughnessColor.r;
    float metallic = metallicColor.r;

    // Calculate reflectance at normal incidence.
    // If dielectric (like plastic) use F0 of 0.04
    // If it's a metal, use the baseColor of the material to define the F0
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, baseColor, metallic);

    vec3 normal = 2.0 * normalColor - 1.0; // Convert normal map from RGB to normal space [-1, 1]
    vec3 N = (normalize(inTBN * normal)); // The vector that is normal to the fragment. (Multiplying by the TBN brings the tangent space normals to world space to match the other lighting vectors)
    vec3 V = normalize(environmentData.cameraWorldPosition.xyz - inFragPosition); // The vector from the camera to the fragment

    vec3 result = vec3(0.0f);

    // Calculate each light's impact on the scene
    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        PointLight light = pointLights.lights[i];
        result += CalculatePointLight(i, light, inFragPosition, baseColor, roughness, metallic, emissivityColor, N, V, F0);
    }

    // Environment reflection
    if (environmentData.skyboxCubemapIndex != 4294967295)
    {
        vec3 I = normalize(inFragPosition - environmentData.cameraWorldPosition.xyz);
        vec3 reflected = reflect(I, N);
        vec3 environmentReflectionColor = SkyboxColor(environmentData.skyboxCubemapIndex, reflected);

        result += (F0 * (1.0f - roughness) * environmentReflectionColor);
    }

    // Ambient lighting
    for (int i = 0; i < pointLights.lights.length(); i++)
    {
        if (pointLights.lights[i].isInitialized == 0)
        {
            break;
        }

        PointLight light = pointLights.lights[i];

        vec3 ambient = vec3(0.03) * light.ambientColor * ambientOcclusionColor;
        result += ambient;
        result = max(result, ambient);
    }

    outFragColor = vec4(result, 1.0);
}