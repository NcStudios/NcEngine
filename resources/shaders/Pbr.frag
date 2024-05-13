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

// D: Normal Distribution Function (GGX)
float D(float roughness, float NdotH)
{
    float roughnessSqr = roughness*roughness;
    float NdotHSqr = NdotH*NdotH;
    float TanNdotHSqr = (1-NdotHSqr)/NdotHSqr;
    return (1.0/PI) * pow(roughness/(NdotHSqr * (roughnessSqr + TanNdotHSqr)), 2);
}

// G: Geometry Shadowing Function (Schlick-Beckmann)
float G(float NDotL, float NDotV, float roughness)
{
    float roughnessSqr = roughness * roughness;
    float k = roughnessSqr * 0.79788456;
    float smithL = (NDotL) / (NDotL * (1 - k) + k);
    float smithV = (NDotV) / (NDotV * (1 - k) + k);
    return smithL * smithV;
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

// vec3 CalculatePointLight(int lightIndex, vec3 N, vec3 V, vec3 F0, vec3 baseColor, float roughness, float metallic, vec4 lightViewPos)
// {
//     PointLight light = pointLights.lights[lightIndex];

//     // Per light radiance
//     vec3 L = normalize(light.position - inFragPosition);
//     vec3 H = normalize(V + L);
//     float distance = length(light.position - inFragPosition);
//     float attenuation = (1.0 / (distance * distance)) * (light.radius * 0.8);
//     vec3 radiance = light.diffuseColor * attenuation;

//     // Cook-Torrance BRDF
//     float NDF = DistributionGGX(N, H, roughness);
//     float G = GeometrySmith(N, V, L, roughness);
//     vec3 F = FresnelSchlick(max(dot(H,V), 0.0), F0);

//     vec3 numerator = NDF * G * F;
//     float denominator = 4 * max(dot(N,V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
//     vec3 specular = numerator / denominator;

//     vec3 kS = F;
//     vec3 kD = vec3(1.0) - kS;
//     kD *= 1.0 - metallic;

//     float NDotL = max(dot(N, L), 0.0);

//     vec3 colorTotal = (kD * baseColor / PI + specular) * radiance * NDotL;

//     float shadow = 0.0;

//     // Shadow
//     if (light.castsShadows == 1)
//     {
//         shadow = ShadowCalculation(lightViewPos, lightIndex);
//     }

//     return (1.0 - shadow) * colorTotal;
// }

const mat4 biasMat = mat4( 
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 
);

void main() 
{
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
    D                | GGX      Normal Distribution Function                           | NDF - The distribution of microfacet normals compared to the half vector, aka, the roughness
    G                | Schlick-Beckmann Geometry Shadowing Function                    | GSF - The attenuation of light due to microfacet self-shadowing
    F                | F₀ + (1 - F₀)(1 - (V ⋅ H))^5                                    | Fresnel function, but it's already in our specularK so we don't need it twice.
    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    **/

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

    // The following needs to be repeated per light
    PointLight light = pointLights.lights[0];
    vec3 L = normalize(light.position - inFragPosition); // The vector from the light to the fragment. Note, for directional lights, L would just be the position.
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
    float distance = length(light.position - inFragPosition);
    // float attenuation = clamp(1.0 - distance * distance / (light.radius * light.radius), 0.0, 1.0);
    float attenuation = clamp(1/(pow(distance, 4.0f)), 0.0, 1.0);
    vec3 radiance = light.diffuseColor * attenuation * pow(light.radius, 3);

    vec3 result = emissivityColor + brdf * radiance * LDotN;

    float shadow = 0.0;

    // Shadow
    if (light.castsShadows == 1)
    {
        shadow = ShadowCalculation(biasMat * light.viewProjection * vec4(inFragPosition, 1.0), 0);
    }

    result *=  (1.0 - shadow);


    // vec3 result = vec3(0.0);
    // for (int i = 0; i < pointLights.lights.length(); i++)
    // {
    //     if (pointLights.lights[i].isInitialized == 0)
    //     {
    //         break;
    //     }

    //     vec4 lightViewPos = vec4(1.0f);
    //     if (pointLights.lights[i].castsShadows == 1)
    //     {
    //         lightViewPos = biasMat * pointLights.lights[i].viewProjection * vec4(inFragPosition, 1.0);
    //     }

    //     result += CalculatePointLight(i, N, V, F0, baseColor, roughnessColor.r, metallicColor.r, lightViewPos) + (pointLights.lights[i].ambientColor * 0.015);
    // }

   if (environmentData.skyboxCubemapIndex != 4294967295)
    {
        // Environment reflection
        vec3 I = normalize(inFragPosition - environmentData.cameraWorldPosition.xyz);
        vec3 reflected = reflect(I, N);
        vec3 environmentReflectionColor = SkyboxColor(environmentData.skyboxCubemapIndex, reflected);

        result += (F0 * (1.0f - metallic) * environmentReflectionColor) / 2;
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
    }


    outFragColor = vec4(result, 1.0);
}